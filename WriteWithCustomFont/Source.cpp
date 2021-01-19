#include <string>
#include <iostream>

#include "custom_font.h"

int main()
{
    fonts::CustomFont font;
    if (!font.Load(font.ChooseManually()))
    {
        return 1;
    }
    
    std::cout << std::endl << "Your text to be written with custom font: ";
    std::string text;
    std::getline(std::cin, text);
    font.WriteMessage(text);
    
    return 0;
}
