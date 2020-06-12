#include "list.h"

#include <string>
#include <conio.h>
#include <iostream>
#include <unordered_map>

static const char* FileForSerialization{"C:\\Temp\\test.txt"};

#ifdef _WIN32
#define GETCH _getch()
#else
#define GETCH getch()
#endif // _WIN32

int main()
{
    List list;
    bool doQuit{false};

    while (!doQuit)
    {
        std::cout << "Please choose action: " << std::endl
            << "1 - Add Nodes to List" << std::endl
            << "2 - Pop last Node from List" << std::endl
            << "3 - Serialize List" << std::endl
            << "4 - Deserialize List" << std::endl
            << "5 - Show List" << std::endl;

        int chosenAction = 0;
        bool isNotValidAction = true;
        while (isNotValidAction)
        {
            FILE* file = nullptr;
            errno_t err;
            std::string value;
            chosenAction = GETCH;
            switch (chosenAction)
            {
            case '1': // Add
                std::cout << "Enter the value to add to the List (empty by default)" << std::endl;
                std::cin >> value;
                list.Add(value);
                break;
            case '2': // Pop
                list.Pop();
                break;
            case '3': // Serialize
                err = fopen_s(&file, FileForSerialization, "wb");
                if (err != 0)
                {
                    printf("Could not create or open file: \"%s\"\n", FileForSerialization);
                    break;
                }
                list.Serialize(file);
                fclose(file);
                std::cout << std::endl << "Serialized list to " << FileForSerialization << std::endl;
                break;
            case '4': // Deserialize
                err = fopen_s(&file, FileForSerialization, "rb");
                if (err != 0)
                {
                    printf("Could not open file: \"%s\"\n", FileForSerialization);
                    break;
                }
                list.Deserialize(file);
                fclose(file);
                std::cout << std::endl << "Deserialized list from " << FileForSerialization << std::endl;
                break;
            case '5':
                std::cout << std::endl << "List contains: " << std::endl;
                for (auto it = list.head; it; it = it->next)
                {
                    std::cout << "value " << it->data << "; random pointer points to node with value " << (it->rand ? it->rand->data : "NULL") << std::endl;
                }
                break;
            default:
                std::cout << "Unknown action. Please choose action from above" << std::endl;
                continue;
            }
            std::cout << std::endl;
            isNotValidAction = false;
        }
    }
}