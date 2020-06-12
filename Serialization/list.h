#ifndef LIST_HEADER
#define LIST_HEADER

#include <string>

struct ListNode
{
    ListNode* prev;
    ListNode* next;
    ListNode* rand; // reference to other random ListNode element, or nullptr
    std::string data;
};

class List
{
public:
    List();

    void Serialize(FILE* file); // save to file (file is opened via fopen(path, "wb"))
    void Deserialize(FILE* file); // save to file (file is opened via fopen(path, "rb"))

    void Pop();
    void Add(const std::string& data);

private:
    ListNode* InsertNode(const std::string& data);
    static char BinToChar(const char* str);

public:
    ListNode* head;
    ListNode* tail;
    unsigned count;
};

#endif // LIST_HEADER