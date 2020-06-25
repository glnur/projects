#include "list.h"

#include <string>
#include <conio.h>
#include <iostream>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

static std::string FileForSerialization{"C:\\Temp\\test.txt"};

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
            << "3 - Save List to file" << std::endl
            << "4 - Load List from file" << std::endl
            << "5 - Show List" << std::endl
            << "6 - Change file List to Save to/Load from. Current - " << FileForSerialization << std::endl
            << "7 - Exit" << std::endl;

        int chosenAction = 0;
        bool isNotValidAction = true;
        while (isNotValidAction)
        {
            FILE* file = nullptr;
            errno_t err;
            chosenAction = GETCH;
            switch (chosenAction)
            {
            case '1': // Add
            {
                std::string valueToAdd;
                std::cout << std::endl << "Enter the value to add to the List (empty by default)" << std::endl;
                std::cin >> valueToAdd;
                list.Add(valueToAdd);
                break;
            }
            case '2': // Pop
            {
                list.Pop();
                break;
            }
            case '3': // Serialize
            {
                err = fopen_s(&file, FileForSerialization.c_str(), "wb");
                if (err != 0)
                {
                    printf("Could not create or open file: \"%s\"\n", FileForSerialization);
                    break;
                }
                list.Serialize(file);
                fclose(file);
                std::cout << std::endl << "Serialized list to " << FileForSerialization << std::endl;
                break;
            }
            case '4': // Deserialize
            {
                err = fopen_s(&file, FileForSerialization.c_str(), "rb");
                if (err != 0)
                {
                    printf("Could not open file: \"%s\"\n", FileForSerialization);
                    break;
                }
                list.Deserialize(file);
                fclose(file);
                std::cout << std::endl << "Deserialized list from " << FileForSerialization << std::endl;
                break;
            }
            case '5': // Show List
            {
                std::cout << std::endl << "List contains: " << std::endl;
                if (list.count == 0)
                {
                    std::cout << "List is empty" << std::endl;
                    break;
                }

                for (auto it = list.head; it; it = it->next)
                {
                    std::cout << "value " << it->data << "; random pointer points to node with value " << (it->rand ? it->rand->data : "NULL") << std::endl;
                }
                break;
            }
            case '6': // Change file for serialization
            {
                std::cout << std::endl << "Please enter full(absolute, for example C:\\Test\\test.txt) file path (or leave line empty not to change current file)" << std::endl;

                bool isPathAbsolute = false;
                while (!isPathAbsolute)
                {
                    std::cin.ignore();
                    std::string filePathString;
                    getline(std::cin, filePathString);
                    if (filePathString.empty()) // leave current file
                    {
                        break;
                    }

                    const fs::path filePath{boost::trim_copy(filePathString)};
                    if (!filePath.is_absolute())
                    {
                        std::cout << "You entered invalid file path. Please enter FULL file path" << std::endl;
                        continue;
                    }

                    boost::system::error_code ec;
                    fs::create_directories(filePath.parent_path(), ec);
                    if (!fs::is_directory(filePath.parent_path(), ec))
                    {
                        std::cout << "Could not create path to " << filePath << ". Error: " << ec.message() << std::endl;
                        break;
                    }

                    isPathAbsolute = true;
                    FileForSerialization = filePath.string();
                }
                break;
            }
            case '7': // Exit
            {
                std::cout << "Exiting..." << std::endl;
                doQuit = true;
                break;
            }
            default:
            {
                std::cout << "Unknown action. Please choose action from above" << std::endl;
                continue;
            }
            }
            std::cout << std::endl;
            isNotValidAction = false;
        }
    }

    return 0;
}