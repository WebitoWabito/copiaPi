#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>

#include "SSLSocket.h"

#define BUFSIZE 4096

using namespace std;

string buildRequest(const string &cmd, const string &msg = "")
{
  return "P/" + cmd + "/" + msg;
}

int main(int argc, char *argv[])
{
  char buffer[BUFSIZE];

  const char *serverIP;
  int port;

  if (argc < 3)
  {
    cerr << "Uso: " << argv[0] << " IP PUERTO\n";
    return 1;
  }

  if (argc >= 2)
  {
    serverIP = argv[1];
  }

  if (argc >= 3)
  {
    port = atoi(argv[2]);

    if (port <= 0 || port > 65535)
    {
      cerr << "Puerto invalido: " << argv[2] << endl;
      cerr << "Uso: " << argv[0] << " [IP] [PUERTO]\n";
      return 1;
    }
  }

  try
  {
    SSLSocket client(false);
    client.Connect(serverIP, port);

    cout << "Conectado al servidor mediante SSL/TLS\n";
    cout << "Servidor: " << serverIP << ":" << port << "\n";
    cout << "Cifrado: " << client.GetCipher() << "\n";

    while (true)
    {
      cout << "\n===== CLIENTE =====\n";
      cout << "1. Listar figuras\n";
      cout << "2. Obtener figura\n";
      cout << "3. Salir\n";
      cout << "Seleccione: ";

      int option;
      cin >> option;
      cin.ignore();

      string req;

      switch (option)
      {
      case 1:
        req = buildRequest("R", "dir");
        break;

      case 2:
      {
        string name;
        cout << "Nombre de la figura: ";
        getline(cin, name);
        req = buildRequest("G", name);
        break;
      }

      case 3:
        req = buildRequest("Q");
        client.Write(req.c_str());
        cout << "Conexion cerrada.\n";
        return 0;

      default:
        cout << "Opcion invalida\n";
        continue;
      }

      client.Write(req.c_str());

      memset(buffer, 0, sizeof(buffer));
      int n = client.Read(buffer, sizeof(buffer) - 1);

      if (n <= 0)
      {
        cout << "Conexion cerrada por el servidor\n";
        break;
      }

      string res(buffer, n);

      if (res.rfind("P/D/", 0) == 0)
      {
        string data = res.substr(4);

        if (data == "404")
        {
          cout << "Figura no encontrada\n";
        }
        else if (data.empty())
        {
          cout << "No hay datos disponibles\n";
        }
        else
        {
          for (char &c : data)
          {
            if (c == ',')
              c = '\n';
          }

          cout << "\n===== RESPUESTA =====\n";
          cout << data << endl;
        }
      }
      else if (res.rfind("P/A/", 0) == 0)
      {
        cout << res.substr(4) << endl;
      }
      else
      {
        cout << "Respuesta desconocida:\n"
             << res << endl;
      }
    }
  }
  catch (const exception &e)
  {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}