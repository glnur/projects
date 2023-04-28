#include "list.h"
#include <bitset>
#include <iostream>
#include <unordered_map>

static constexpr size_t SizeTSize{sizeof(size_t) * 8};

List::List()
{
    head = tail = nullptr;
    count = 0;
}

void List::Serialize(FILE* file) // save to file (file is opened via fopen(path, "wb"))
{
    const std::string binaryCount{std::bitset<sizeof(decltype(count)) * 8>(count).to_string()};
    fwrite(binaryCount.c_str(), sizeof(char), binaryCount.size(), file);

    const char endLine[]{ "\n" };
    fwrite(endLine, sizeof(char), strlen(endLine), file);

    // Numerate each node
    std::unordered_map<ListNode*, unsigned> nodeId;
    unsigned id = 0;
    for (auto it = head; it; it = it->next)
    {
        nodeId.insert({ it, id++ });
    }

    // Save data in file
    for (auto it = head; it; it = it->next)
    {
        std::string binaryString{ "" };
        for (const char& letter : it->data)
        {
            binaryString += std::bitset<8>(letter).to_string();
        }
        fwrite(binaryString.c_str(), sizeof(char), binaryString.size(), file);
        fwrite(";", sizeof(char), strlen(endLine), file);

        if (it->rand)
        {
            std::string binaryRandId{ std::bitset<SizeTSize>(nodeId[it->rand]).to_string() };
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
        InsertNode(std::string{});
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
        std::string data{ BinToChar(myChar) };
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

void List::Pop()
{
    // TODO if Insert() func to any place in the list will be implemented
    // Need to validate each ListNode's rand

    if (count == 0)
    {
        std::cout << "Nothing to pop. List is empty" << std::endl;
        return;
    }

    std::cout << "Popping Node with value " << tail->data << std::endl;
    if (tail == head) // only 1 ListNode in List
    {
        delete tail;
        head = tail = nullptr;
    }
    else
    {
        tail = tail->prev;
        delete tail->next;
        tail->next = nullptr;
    }
    --count;
}

void List::Add(const std::string& data)
{
    ListNode* newNode = InsertNode(data);

    // Initialize rand pointer with randomly chosen ListNode
    const unsigned randId = rand() % (count + 1); // + 1 to count for nullptr
    newNode = head; // pointer for tail->rand to point
    tail->rand = nullptr;

    if (randId >= count)
    {
        tail->rand = nullptr;
    }
    else
    {
        for (unsigned i = 0u; i != randId; ++i)
        {
            if (newNode->next)
            {
                newNode = newNode->next;
            }
            else
            {
                tail->rand = nullptr;
                std::cout << "Could not itialize random pointer properly. Initialized it with nullptr instead" << std::endl;
                return;
            }
        }
        tail->rand = newNode;
    }
}

ListNode* List::InsertNode(const std::string& data)
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

    return newNode;
}

/*static*/ char List::BinToChar(const char* str)
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
