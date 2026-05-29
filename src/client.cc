#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>  

#include "VSocket.h"
#include "SSLSocket.h"

using namespace std;

int main()
{
    bool IPV6;
    char ip;

    while (true)
    {

        cout << "Quiere usar ipv4(4) o ipv6(6) ";
        cin >> ip;

        if (ip == '4')
        {
            IPV6 = false;
            break;
        }
        else if (ip == '6')
        {
            IPV6 = true;
            break;
        }
        else
        {
            continue;
        }
    }

    const char *ose = "163.178.104.62";

    // Para ver las opciones disponibles
    vector<string> figures = {
        "horse", "blacksheep", "dragon", "duck", "elephant",
        "fish", "giraffe", "lion", "monkey", "orchid",
        "penguin", "roadrunner", "shark", "squid",
        "swan", "turtle", "whitesheep"
    };

    while (true)
    {
        int totalPieces = 0;
        bool error = false;

        string input;

        cout << endl << "Seleccione una figura:" << endl;
        for (int i = 0; i < figures.size(); i++)
        {
            cout << i + 1 << ") " << figures[i] << endl;
        }
        cout << "0) Salir" << endl;

        int option;

        if (!(cin >> option)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Opcion invalida" << endl;
            continue;
        }

        if (option == 0)
        {
            break;
        }

        if (option < 1 || option > figures.size())
        {
            cout << "Opcion invalida" << endl;
            continue;
        }

        input = figures[option - 1];

        for (int part = 1; part <= 2; part++)
        {
            string request = "GET /lego/list.php?figure=" + input +
                             "&part=" + to_string(part) +
                             " HTTP/1.1" + "\r\n";

            request += "Host: redes.ecci.ucr.ac.cr\r\n";
            request += "Connection: close\r\n\r\n";

            VSocket *s;
            if (IPV6)
            {
                s = new SSLSocket(true);
                s->Connect("fe80::8f5a:e2e1:7256:ffe3%enp0s31f6", "443");
            }
            else
            {
                s = new SSLSocket(false);
                s->Connect(ose, 443);
            }

            s->Write(request.c_str());

            char buffer[512];
            int bytes;
            string response = "";

            while ((bytes = s->Read(buffer, sizeof(buffer) - 1)) > 0)
            {
                buffer[bytes] = '\0';
                response += buffer;
            }

            size_t lineEnd = response.find("\r\n");
            string statusLine = response.substr(0, lineEnd);

            if (statusLine.find("404") != string::npos)
            {
                cout << "Lo sentimos, esa figura no existe" << endl;
                error = true;
                break;
            }
            size_t bodyStart = response.find("\r\n\r\n");
            if (bodyStart == string::npos)
            {
                cout << "Error en respuesta" << endl;
                continue;
            }

            string html = response.substr(bodyStart + 4);

            size_t currentPosition = 0;

            while ((currentPosition = html.find("<TR><TD ALIGN=center>", currentPosition)) != string::npos)
            {

                size_t rowEnd = html.find("</TR>", currentPosition);
                if (rowEnd == string::npos)
                    break;

                string row = html.substr(currentPosition, rowEnd - currentPosition);

                size_t amountTagPos = row.find("<TD ALIGN=center>");
                size_t amountStartPos = row.find(">", amountTagPos) + 1;
                size_t amountEndPos = row.find("</TD>", amountStartPos);
                string amount = row.substr(amountStartPos, amountEndPos - amountStartPos);
                int amountInteger = stoi(amount);
                totalPieces += amountInteger;

                size_t descriptionTagPos = row.find("<TD ALIGN=center>", amountEndPos);
                size_t descriptionStartPos = row.find(">", descriptionTagPos) + 1;
                size_t descriptionEndPos = row.find("</TD>", descriptionStartPos);
                string description = row.substr(descriptionStartPos, descriptionEndPos - descriptionStartPos);

                cout << amount << " " << description << endl;

                currentPosition = rowEnd;
            }
        }

        if (!error)
        {
            cout << "Total de piezas para armar esta figura " << totalPieces << endl;
        }

        cout << endl << "Presione ENTER para continuar...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Agregué esto para pausar las figuras, antes de mostrar
        cin.get();                                          // las opciones de nuevo
    }

    return 0;
}