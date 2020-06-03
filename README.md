# projects
Examples of interesting tasks I have done

1)
Name: Double linked list Serialization/Deserialization in binary format.

Description:
We have a List class that contains head and tail pointers to ListNode objects.
ListNode object contains pointers to previous, next and random ListNode object and string value named data.
By calling .Serialize() method, we save current List object state in the file in binary format.
After that we can do whatever we want with a List object:
Add more ListNodes
Pop ListNodes from the end
Edit data/pointers in ListNodes
By calling .Deserialize() method, we recover saved List object state from the file,
I.e all our actions with List object after last .Serialize() method call will be reversed.

2)
Name: Printing text with custom font

Description:
There are 3 fonts in fonts.xml to choose from.
You are free to add your custom fonts.
On the start all fonts' examples will be printed for "abc" text.
After choosing which font do you want to use,
Type a text (sentence or word) that later will be printed with a chosen font.
