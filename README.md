# projects
Examples of interesting tasks I have done

1)
Name: Double linked list Serialization/Deserialization in binary format.

Description:
We have a List class that contains head and tail pointers to ListNode objects.
ListNode object contains pointers to previous, next and random ListNode object and string value named data.
By calling .Serialize() method, we save current List object state in the file in binary format.
After that we can do whatever we want with a List object:
1) Add more ListNodes
2) Pop ListNodes from the end
3) Serialize/Deserialize List
4) Show List content

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

3)
Name: Running bot

Description:
There are a player and bot on the map.
You can control the player with WASD.
Bot is always chasing the player.
In this version there is no end in the game, just press ESC when you want to exit.
In the next commit obstacles will be added to map and bot will start to use Dijkstra's algorithm to move.
