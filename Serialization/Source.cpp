#include <iostream>
#include <bitset>
#include <string>
#include <unordered_map>

static const char* FileForSerialization{"C:\\Temp\\test.txt"};
static const size_t SizeTSize{sizeof(size_t) * 8};

struct ListNode 
{
    ListNode* prev;
    ListNode* next;
    ListNode* rand; // reference to other random ListNode element, or NULL
    std::string data;
};

class List 
{
public:
    List()
    {
        head = tail = nullptr;
        count = 0;
    }

    void Serialize(FILE* file); // save to file (file is opened via fopen(path, "wb"))

    void Deserialize(FILE* file); // save to file (file is opened via fopen(path, "rb"))

    void Pop()
    {
        if (tail)
        {
            tail = tail->prev;
            if (tail->rand == tail->next)
            {
                tail->rand = nullptr;
            }
            tail->next = nullptr;
        }
    }

    void Add(const std::string& data)
    {
        ListNode* newNode = new ListNode;
        newNode->next = nullptr;
        newNode->rand = nullptr;
        newNode->data = data;

        if (head)
        {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        else
        {
            newNode->prev = nullptr;
            head = tail = newNode;
        }

        ++count;
    }

    void Add()
    {
        Add(std::string{});
    }
    
private:
    char BinToChar(const char* str)
    {
        char letter = 0;
        for (int i = 0; i < 8; i++)
        {
            if (str[i] == '1')
            {
                letter |= 1 << (7 - i);
            }
        }
        return letter;
    }

public:
    ListNode* head;
    ListNode* tail;
    unsigned count;
};

void List::Serialize(FILE* file) // save to file (file is opened via fopen(path, "wb"))
{
    std::string binaryCount{std::bitset<sizeof(decltype(count)) * 8>(count).to_string()};
    fwrite(binaryCount.c_str(), sizeof(char), binaryCount.size(), file);

    const char endLine[]{"\n"};
    fwrite(endLine, sizeof(char), strlen(endLine), file);

    // Numerate each node
    std::unordered_map<ListNode*, unsigned> nodeId;
    unsigned id = 0;
    for (auto it = head; it; it = it->next)
    {
        nodeId.insert({it, id++});
    }

    // Save data in file
    for (auto it = head; it; it = it->next)
    {
        std::string binaryString{""};
        for (const char& letter: it->data)
        {
            binaryString += std::bitset<8>(letter).to_string();
        }
        fwrite(binaryString.c_str(), sizeof(char), binaryString.size(), file);
        fwrite(";", sizeof(char), strlen(endLine), file);
    
        if (it->rand)
        {
            std::string binaryRandId{std::bitset<SizeTSize>(nodeId[it->rand]).to_string()};
            fwrite(binaryRandId.c_str(), sizeof(char), binaryRandId.size(), file);
        }
        fwrite(endLine, sizeof(char), strlen(endLine), file);
    
        fflush(file);
    }
}

void List::Deserialize(FILE* file) // save to file (file is opened via fopen(path, "rb"))
{
    // Array size for string to read size_t value from file
    const size_t arraySize = SizeTSize + 1;

    // Read length
    char lengthBinaryStr[arraySize] = "";
    size_t length = 0;
    if (fgets(lengthBinaryStr, arraySize, file) == NULL)
    {
        return;
    }
    length = std::bitset<SizeTSize>(lengthBinaryStr).to_ulong();
    if (length == 0)
    {
        return;
    }
    fgetc(file); //  to skip end-of-line symbol "\n"

    // Create list full of dummys equal to saved length
    head = tail = nullptr;
    std::vector<ListNode*> nodes;
    nodes.reserve(length);
    for (unsigned int i = 0; i < length; ++i)
    {
        Add();
        nodes.push_back(tail);
    }

    // Fill each dummy with saved info
    for (auto it = head; it; it = it->next)
    {
        // Read first 8 chars (first letter)
        char myChar[9] = "";
        if (fgets(myChar, 9, file) == NULL)
        {
            break;
        }

        // Then read until ';' character
        std::string data{BinToChar(myChar)};
        char possibleSeparationChar = fgetc(file); //  to skip separation symbol ";"
        while (possibleSeparationChar != ';')
        {
            char withoutFirstBit[8] = "";
            if (fgets(withoutFirstBit, 8, file) == NULL)
            {
                break;
            }

            std::string binaryLetterStr = possibleSeparationChar + std::string(withoutFirstBit);
            data += BinToChar(binaryLetterStr.c_str());
            possibleSeparationChar = fgetc(file); //  to skip separation symbol ";"
        }
        it->data = data;

        // Read rand id
        char randIdStr[arraySize] = "";
        if (fgets(randIdStr, arraySize, file) == NULL)
        {
            break;
        }

        if (strlen(randIdStr) == SizeTSize) // if there is a saved rand reference
        {
            unsigned long randId = std::bitset<SizeTSize>(randIdStr).to_ulong();
            it->rand = nodes[randId];
            fgetc(file); //  to skip end-of-line symbol "\n"
        }
    }
}

char BinToChar(const char* str)
{
    char letter = 0;
    for (int i = 0; i < 8; i++)
    {
        if (str[i] == '1')
        {
            letter |= 1 << (7 - i);
        }
    }
    return letter;
}

int main()
{
    List list;

    for (auto i = 0; i < 5; ++i)
    {
        list.Add("item_" + std::to_string(i));
    }

    for (auto it = list.head; it; it = it->next)
    {
        std::cout << "SERIALIZED DATA " << it->data;
        if (it->prev) // just to check rand save is working
        {
            it->rand = it->prev->prev;
        }
        std::cout << "; RAND DATA " << (it->rand ? it->rand->data : "NULL");
        std::cout << std::endl;
    }
    std::cout << std::endl;

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, FileForSerialization, "wb");
    if (err != 0)
    {
        printf("Could not create or open file: \"%s\"\n", FileForSerialization);
        return 1;
    }
    list.Serialize(file);
    fclose(file);

    for (auto i = 5; i < 9; ++i)
    {
        list.Add("item_" + std::to_string(i));
    }
    
    ListNode dummyForRand;
    dummyForRand.data = "dummy";
    int i = 0;
    for (auto it = list.head; it; it = it->next, ++i)
    {
        it->data = "test";
        std::cout << "AFTER SERIALIZE DATA " << it->data;
        if (i % 2)
        {
            it->rand = &dummyForRand;
        }
        std::cout << "; RAND DATA " << (it->rand ? it->rand->data : "NULL");
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    err = fopen_s(&file, FileForSerialization, "rb");
    if (err != 0)
    {
        printf("Could not open file\n");
        return 1;
    }
    
    list.Deserialize(file);
    fclose(file);
    
    for (auto it = list.head; it; it = it->next)
    {
        std::cout << "RECOVERED DATA " << it->data;
        std::cout << "; RAND DATA " << (it->rand ? it->rand->data : "NULL");
        std::cout << std::endl;
    }
}