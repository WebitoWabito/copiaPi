#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <sstream>
#include <chrono>

#include "SSLSocket.h"
#include "FileMon.h"
#include "ProtocolUtils.h"
#include "HTTPUtils.h"

#define PORT 4321
#define BUFSIZE 4096

using namespace std;

mutex mtx;

string getDirectoryListing()
{
    lock_guard<mutex> lock(mtx);

    if (directory.empty())
    {
        return "No hay figuras disponibles\n";
    }

    ostringstream out;

    for (size_t i = 0; i < directory.size(); ++i)
    {
        out << directory[i].name;
        if (i + 1 < directory.size())
        {
            out << "\n";
        }
    }

    return out.str();
}

string getFigureContent(const string &name)
{
    lock_guard<mutex> lock(mtx);

    for (const auto &f : directory)
    {
        if (f.name == name)
        {
            string result;
            int current = f.startCluster;
            int remaining = f.size;

            while (current != END && remaining > 0)
            {
                int bytesToRead = min(CLUSTER_SIZE, remaining);
                result.append(disk[current], bytesToRead);

                remaining -= bytesToRead;
                current = FAT[current];
            }

            return result;
        }
    }

    return "";
}

void handleUppercase(VSocket *client, const string &cmd, const string &msg)
{
    string response;

    if (cmd == "R")
    {
        if (msg == "dir")
        {
            string dir = getDirectoryListing();

            for (char &c : dir)
            {
                if (c == '\n')
                {
                    c = ',';
                }
            }

            response = "P/D/" + dir;
        }
        else
        {
            response = "P/A/Request invalido";
        }
    }
    else if (cmd == "G")
    {
        string data = getFigureContent(msg);

        if (data.empty())
        {
            response = "P/D/404";
        }
        else
        {
            response = "P/D/" + data;
        }
    }
    else if (cmd == "Q")
    {
        response = "P/A/bye";
        client->Write(response.c_str());
        return;
    }
    else
    {
        response = "P/A/Comando invalido";
    }

    client->Write(response.c_str());
}

void handleHTTP(VSocket *client, const string &request)
{
    string path = extractHTTPPath(request);

    if (path == "/" || path == "/dir")
    {
        string body = getDirectoryListing();
        string response = buildHTTPResponse(body);
        client->Write(response.c_str());
        return;
    }

    if (!path.empty() && path[0] == '/')
    {
        path = path.substr(1);
    }

    string content = getFigureContent(path);

    if (content.empty())
    {
        string response = buildHTTPResponse(
            "Figura no encontrada\n",
            "404 Not Found");
        client->Write(response.c_str());
        return;
    }

    string response = buildHTTPResponse(content);
    client->Write(response.c_str());
}

void task(VSocket *client)
{
    char buffer[BUFSIZE];

    try
    {
        while (true)
        {
            memset(buffer, 0, sizeof(buffer));

            int n = client->Read(buffer, sizeof(buffer) - 1);

            if (n <= 0)
            {
                break;
            }

            string request(buffer, n);

            if (isHTTP(request))
            {
                handleHTTP(client, request);
                break;
            }

            string cmd, msg;

            if (!parseRequest(request, cmd, msg))
            {
                client->Write("P/A/Error protocolo");
                continue;
            }

            handleUppercase(client, cmd, msg);

            if (cmd == "Q")
            {
                break;
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Cliente desconectado o error de SSL: \n" << e.what() << endl;
    }

    client->Close();
    delete client;
}

void serverConsole()
{
    while (true)
    {
        cout << "\n===== SERVER MENU =====\n";
        cout << "1. Crear figura\n";
        cout << "2. Listar figuras\n";
        cout << "3. Leer figura\n";
        cout << "4. Salir\n";
        cout << "Seleccione: ";

        int option;
        cin >> option;
        cin.ignore();

        lock_guard<mutex> lock(mtx);

        switch (option)
        {
        case 1:
            createFile();
            break;

        case 2:
            listFiles();
            break;

        case 3:
            readFileFromDirectory();
            break;

        case 4:
            cout << "Cerrando servidor...\n";
            exit(0);

        default:
            cout << "Opcion invalida\n";
        }
    }
}

int main()
{
    VSocket *client;

    Init();

    SSLSocket server(
        (char *)"server.crt",
        (char *)"server.key",
        false);

    cout << "Servidor SSL de figuras iniciado\n";
    cout << "Puerto: " << PORT << "\n";

    server.Bind(PORT);
    server.MarkPassive(10);

    thread(serverConsole).detach();

    while (true)
    {
        client = server.AcceptConnection();

        SSLSocket *sslClient = dynamic_cast<SSLSocket *>(client);

while (true)
{
    VSocket *client = server.AcceptConnection();

    try
    {
        SSLSocket *sslClient =
            dynamic_cast<SSLSocket *>(client);

        if (sslClient)
        {
            sslClient->AttachSSL(server.GetContext());
        }

        std::thread(task, client).detach();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Handshake SSL fallido: "
                  << e.what() << std::endl;

        client->Close();
        delete client;
    }
}

        thread(task, client).detach();
    }

    return 0;
}