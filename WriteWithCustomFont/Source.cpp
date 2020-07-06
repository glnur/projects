#include <set>
#include <map>
#include <string>
#include <sstream>
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <unordered_set>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

static constexpr unsigned TotalSymbols = 27u; // 27th symbol '?' is for non latin symbols

enum class CutomFontName : uint8_t
{
    Hashtag = 0u,
    Cube,
    Avatar
};

class CustomFont
{
public:
    CustomFont()
    {
        boost::system::error_code ec;
        m_xmlFile = (fs::current_path(ec) / "fonts.xml").string();
        if (!fs::is_regular_file(m_xmlFile, ec))
        {
            std::cout << "Could not find file " << m_xmlFile << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    const size_t& GetLength() const
    {
        return m_length;
    }

    const size_t& GetHeight() const
    {
        return m_height;
    }

    const std::string& GetFont() const
    {
        return m_font;
    }

    void ShowAllFontsExamples()
    {
        try
        {
            pt::ptree tree;
            pt::read_xml(m_xmlFile, tree);
            int i = 1;
            for (const auto& fonts : tree.get_child("fonts"))
            {
                m_length = tree.get("fonts." + std::string(fonts.first.data()) + ".<xmlattr>.length", 0);
                std::cout << fonts.first.data() << " - " << i++;
                std::istringstream iss(fonts.second.data());
                std::string row;
                while (getline(iss, row))
                {
                    std::cout << row.substr(0, m_length * 3) << std::endl; // print first 3 letters as example
                }
                std::cout << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Error while reading from xml file '" << m_xmlFile << "' : " << e.what() << std::endl;
            return;
        }
    }

    bool Load(CutomFontName font)
    {
        pt::ptree tree;
        boost::system::error_code ec;
        if (!fs::is_regular_file(m_xmlFile, ec))
        {
            std::cout << "Could not find file " << m_xmlFile << std::endl;
            return false;
        }

        std::string fontName;
        switch (font)
        {
        case CutomFontName::Hashtag:
            fontName = "hashtag";
            break;
        case CutomFontName::Cube:
            fontName = "cube";
            break;
        case CutomFontName::Avatar:
            fontName = "avatar";
            break;
        default:
            std::cout << "Unknown type of font" << std::endl;
            return false;
        }

        try
        {
            pt::read_xml(m_xmlFile, tree);
            m_length = tree.get("fonts." + fontName + ".<xmlattr>.length", 0);
            m_height = tree.get("fonts." + fontName + ".<xmlattr>.height", 0);
            m_font = tree.get("fonts." + fontName, "");
        }
        catch (const std::exception& e)
        {
            std::cout << "Error while reading from xml file '" << m_xmlFile << "' : " << e.what();
            return false;
        }

        if (m_length == 0)
        {
            std::cout << "Please specify your fonts length in " << m_xmlFile << std::endl;
            return false;
        }
        else if (m_height == 0)
        {
            std::cout << "Please specify your fonts height in " << m_xmlFile << std::endl;
            return false;
        }
        else if (boost::trim_copy(m_font).empty())
        {
            std::cout << "Please specify your font in " << m_xmlFile << std::endl;
            return false;
        }

        if (m_font.front() == '\n')
        {
            m_font.erase(m_font.begin());
        }
        if (m_font.back() == '\n')
        {
            m_font.pop_back();
        }

        return true;
    }

private:
    unsigned m_length;
    unsigned m_height;
    std::string m_font;
    std::string m_xmlFile;
};

std::ostream& operator<<(std::ostream& os, CutomFontName font)
{
    switch (font)
    {
    case CutomFontName::Hashtag:
        return os << "Hashtag";
    case CutomFontName::Cube:
        return os << "Cube";
    case CutomFontName::Avatar:
        return os << "Avatar";
    default:
        os << "Unknown type of font" << std::endl;
    }
}

#ifdef _WIN32
    #define GETCH _getch()
#else
    #define GETCH getch()
#endif // _WIN32

int main()
{
    CustomFont font;
    font.ShowAllFontsExamples();
    std::cout << "Please enter the id of which font do you want to choose" << std::endl;

    CutomFontName chosenFont;
    int chosenFontId = 0;
    bool isNotValidFontId = true;
    while (isNotValidFontId)
    {
        chosenFontId = GETCH;
        switch (chosenFontId)
        {
        case '1':
            chosenFont = CutomFontName::Hashtag;
            break;
        case '2':
            chosenFont = CutomFontName::Cube;
            break;
        case '3':
            chosenFont = CutomFontName::Avatar;
            break;
        default:
            std::cout << "Unknown font id. Please choose font from above" << std::endl;
            continue;
        }
        isNotValidFontId = false;
    }
    std::cout << "You have chosen " << chosenFont << " font" << std::endl;

    if (!font.Load(chosenFont))
    {
        return 1;
    }

    std::cout << std::endl << "Your text to be written with custom font: ";
    std::string text;
    std::getline(std::cin, text);

    std::map<int, std::string> asciiToArt;
    std::istringstream iss(font.GetFont());
    std::string row;
    while (getline(iss, row))
    {
        for (size_t i = 0; i < row.size(); i += font.GetLength())
        {
            uint8_t letterId = i / font.GetLength();
            for (size_t j = i; j < i + font.GetLength(); ++j)
            {
                asciiToArt[65 + letterId] += std::string(1, row[j]);
                asciiToArt[97 + letterId] += std::string(1, row[j]);
            }
            asciiToArt[65 + letterId] += "\n";
            asciiToArt[97 + letterId] += "\n";
        }

        // Fill space symbol with GetLength() * GetHeight() ' ' symbols
        asciiToArt[32] += std::string(font.GetLength(), ' ');
        asciiToArt[32] += "\n";
    }

    for (size_t h = 0; h < font.GetHeight(); ++h)
    {
        std::unordered_set<int> uniqueSymbolsInLine;
        for (const auto& symbol : text)
        {
            int symbolId = (int)symbol;
            // Print ? symbol for all non-latin symbols except space
            if ((symbolId < 65 || (symbolId > 90 && symbolId < 95) || symbolId > 122) && symbolId != 32/*space*/)
            {
                symbolId = 123;
            }

            std::string& letter = asciiToArt[symbolId];
            if ((h != 0) && (uniqueSymbolsInLine.find(symbolId) == uniqueSymbolsInLine.end()))
            {
                letter.erase(0, font.GetLength() + 1);
                uniqueSymbolsInLine.insert(symbolId);
            }
            std::cout << letter.substr(0, font.GetLength());
        }
        std::cout << std::endl;
    }

    return 0;
}
