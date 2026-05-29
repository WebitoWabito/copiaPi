#include <iostream>
#include <cstring>
#include <algorithm>

#include "FileMon.h"

using namespace std;

int FAT[MAX_CLUSTERS];
bool bitmap[MAX_CLUSTERS];
char disk[MAX_CLUSTERS][CLUSTER_SIZE];
vector<FileEntry> directory;

void Init()
{
  for (int i = 0; i < MAX_CLUSTERS; i++)
  {
    FAT[i] = END;
    bitmap[i] = false;
    memset(disk[i], 0, CLUSTER_SIZE);
  }
}

int getFreeCluster()
{
  for (int i = 0; i < MAX_CLUSTERS; i++)
  {
    if (!bitmap[i])
    {
      bitmap[i] = true;
      FAT[i] = END;
      memset(disk[i], 0, CLUSTER_SIZE);
      return i;
    }
  }
  return -1;
}

int writeFile(const char *data, int size)
{
  if (size <= 0)
    return -1;

  int firstCluster = -1;
  int prevCluster = -1;
  int offset = 0;

  while (offset < size)
  {
    int newCluster = getFreeCluster();

    if (newCluster == -1)
    {
      cout << "No hay espacio disponible\n";

      if (firstCluster != -1)
      {
        int current = firstCluster;
        while (current != END)
        {
          int next = FAT[current];
          bitmap[current] = false;
          FAT[current] = END;
          memset(disk[current], 0, CLUSTER_SIZE);
          current = next;
        }
      }

      return -1;
    }

    int bytesToCopy = min(CLUSTER_SIZE, size - offset);
    memcpy(disk[newCluster], data + offset, bytesToCopy);

    if (prevCluster != -1)
      FAT[prevCluster] = newCluster;
    else
      firstCluster = newCluster;

    prevCluster = newCluster;
    offset += bytesToCopy;
  }

  FAT[prevCluster] = END;
  return firstCluster;
}

void readFile(int startCluster, int fileSize)
{
  int current = startCluster;
  int remaining = fileSize;

  while (current != END && remaining > 0)
  {
    int bytesToRead = min(CLUSTER_SIZE, remaining);
    cout.write(disk[current], bytesToRead);

    remaining -= bytesToRead;
    current = FAT[current];
  }

  cout << endl;
}

void deleteFile(const string &name)
{
  for (auto it = directory.begin(); it != directory.end(); ++it)
  {
    if (it->name == name)
    {
      int current = it->startCluster;

      while (current != END)
      {
        int next = FAT[current];

        bitmap[current] = false;
        FAT[current] = END;
        memset(disk[current], 0, CLUSTER_SIZE);

        current = next;
      }

      directory.erase(it);
      cout << "Archivo eliminado correctamente\n";
      return;
    }
  }

  cout << "Archivo no encontrado\n";
}

void createFile()
{
  string name, content;

  cout << "Nombre del archivo: ";
  cin >> name;
  cin.ignore();

  for (const auto &file : directory)
  {
    if (file.name == name)
    {
      cout << "Ya existe un archivo con ese nombre\n";
      return;
    }
  }

  cout << "Contenido: ";
  getline(cin, content);

  int start = writeFile(content.c_str(), content.size());

  if (start != -1)
  {
    directory.push_back({name, start, (int)content.size()});
    cout << "Archivo creado correctamente\n";
  }
}

void listFiles()
{
  if (directory.empty())
  {
    cout << "No hay archivos\n";
    return;
  }

  cout << "\nArchivos almacenados:\n";

  for (size_t i = 0; i < directory.size(); i++)
  {
    cout << i << ". "
         << directory[i].name
         << " (" << directory[i].size << " bytes)"
         << endl;
  }
}

void readFileFromDirectory()
{
  listFiles();

  if (directory.empty())
    return;

  int index;
  cout << "Seleccione archivo: ";
  cin >> index;

  if (index >= 0 && index < (int)directory.size())
  {
    cout << "\nContenido:\n";
    readFile(directory[index].startCluster,
             directory[index].size);
  }
  else
  {
    cout << "Indice invalido\n";
  }
}

void deleteFileFromDirectory()
{
  listFiles();

  if (directory.empty())
    return;

  int index;
  cout << "Seleccione archivo a eliminar: ";
  cin >> index;

  if (index >= 0 && index < (int)directory.size())
  {
    string name = directory[index].name;
    deleteFile(name);
  }
  else
  {
    cout << "Indice invalido\n";
  }
}

void menu()
{
  int option;

  do
  {
    cout << "\n===== FILE SYSTEM FAT =====\n";
    cout << "1. Crear archivo\n";
    cout << "2. Listar archivos\n";
    cout << "3. Leer archivo\n";
    cout << "4. Eliminar archivo\n";
    cout << "5. Salir\n";
    cout << "Seleccione: ";

    cin >> option;
    cin.ignore();

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
      deleteFileFromDirectory();
      break;

    case 5:
      cout << "Saliendo\n";
      break;

    default:
      cout << "Opcion invalida\n";
    }

  } while (option != 5);
}