#include <conio.h>
#include <iostream>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "custom_font.h"

#ifdef _WIN32
#define GETCH _getch()
#else
#define GETCH getch()
#endif // _WIN32

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

static constexpr const char* WarningMsg{"WARNING: Custom font is not loaded."};

namespace fonts
{

CustomFont::CustomFont()
{
    m_font = std::string();
    m_length = 0u;

    boost::system::error_code ec;
    m_xmlFile = (fs::current_path(ec) / "fonts.xml").string();
    if (!fs::is_regular_file(m_xmlFile, ec))
    {
        std::cout << "Could not find file " << m_xmlFile << std::endl;
        exit(EXIT_FAILURE); // TODO: replace exit
    }
    pt::read_xml(m_xmlFile, m_tree);
    m_fontList.resize(m_tree.get_child("fonts").size());
}

std::string CustomFont::ChooseManually()
{
    ShowAllFontsExamples();

    if (s_isChoosing)
    {
        std::cout << "Another manual choosing is in progress. Please finish it first" << std::endl;
        return std::string();
    }
    s_isChoosing = true;

    std::cout << "Please enter the id of which font do you want to choose" << std::endl;

    int chosenFontId{0};
    bool isValidFontId{false};
    while (!isValidFontId)
    {
        chosenFontId = GETCH - 48/*49 is '1' in ascii*/;
        if (chosenFontId > m_fontList.size())
        {
            std::cout << "Unknown font id. Please choose font from above" << std::endl;
            continue;
        }
        isValidFontId = true;
    }

    std::cout << "You have chosen " << m_fontList[--chosenFontId] << " font" << std::endl;
    s_isChoosing = false;
    return m_fontList[chosenFontId];
}

bool CustomFont::Load(const std::string fontName)
{
    if (fontName.empty())
    {
        std::cout << "Please specify your font name when loading it." << std::endl;
        return false;
    }

    boost::system::error_code ec;
    if (!fs::is_regular_file(m_xmlFile, ec))
    {
        std::cout << "Could not find file " << m_xmlFile << std::endl;
        return false;
    }

    try
    {
        m_length = m_tree.get("fonts." + fontName + ".<xmlattr>.length", 0);
        m_font = m_tree.get("fonts." + fontName, "");
    }
    catch (const std::exception& e)
    {
        std::cout << "Error while reading from xml file '" << m_xmlFile << "' : " << e.what();
        return false;
    }

    if (m_length == 0)
    {
        std::cout << "Please specify your font's length in " << m_xmlFile << std::endl;
        return false;
    }
    else if (boost::trim_copy(m_font).empty())
    {
        std::cout << "Please specify your font in " << m_xmlFile << std::endl;
        return false;
    }

    // Not using boost::trim because there letters can start with space symbols
    if (m_font.front() == '\n')
    {
        m_font.erase(m_font.begin());
    }
    if (m_font.back() == '\n')
    {
        m_font.pop_back();
    }

    return m_isLoaded = true;
}

void CustomFont::WriteMessage(const std::string& message)
{
    std::map<int, std::string> asciiToArt;
    std::istringstream allLetters{GetFont()};
    std::string row;
    bool isFirstLine{true};
    while (getline(allLetters, row))
    {
        for (size_t i = 0u; i < row.size(); i += GetLength())
        {
            uint8_t letterId = i / GetLength();

            for (size_t j = i; j < i + GetLength(); ++j)
            {
                // both lower-case and upper-case letters will be printed in upper-case
                asciiToArt[65 + letterId] += std::string(1, row[j]);
                asciiToArt[97 + letterId] += std::string(1, row[j]);
            }
            // 65 is 'a' and 97 is 'A' in ascii
            asciiToArt[65 + letterId] += "\n";
            asciiToArt[97 + letterId] += "\n";
        }

        // Whitespace between letters is one empty custom font's length symbol
        asciiToArt[32] += std::string(GetLength(), ' ');
        asciiToArt[32] += "\n";

        std::unordered_set<int> uniqueSymbolsInLine;
        for (const auto& symbol: message)
        {
            int symbolId{(int)symbol};
            // Print ? symbol for all non-latin symbols except space
            if ((symbolId < 65 || (symbolId > 90 && symbolId < 95) || symbolId > 122) && symbolId != 32/*space*/)
            {
                symbolId = 123;
            }

            std::string& letter = asciiToArt[symbolId];
            // Our letter string is just one long line full of symbols,
            // So on every row we print exactly GetLength() symbols and erase previous GetLength() number of symbols on the next row
            if (!isFirstLine && (uniqueSymbolsInLine.find(symbolId) == uniqueSymbolsInLine.end()))
            {
                letter.erase(0, GetLength() + 1);
                uniqueSymbolsInLine.insert(symbolId);
            }
            std::cout << letter.substr(0, GetLength());
        }
        std::cout << std::endl;
        isFirstLine = false;
    }
}

void CustomFont::ShowAllFontsExamples() noexcept
{
    try
    {
        unsigned fontId{1u}, fontLength{0u};
        for (const auto& fonts: m_tree.get_child("fonts"))
        {
            fontLength = m_tree.get("fonts." + std::string(fonts.first.data()) + ".<xmlattr>.length", 0);
            m_fontList[fontId - 1] = fonts.first.data();
            std::cout << fonts.first.data() << " - " << fontId++;
            std::istringstream iss(fonts.second.data());
            std::string row;
            while (getline(iss, row))
            {
                std::cout << row.substr(0, fontLength * 3) << std::endl; // print first 3 letters as example
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

const size_t& CustomFont::GetLength() const
{
    if (!m_isLoaded)
    {
        std::cout << WarningMsg << std::endl;
    }

    return m_length;
}

const std::string& CustomFont::GetFont() const
{
    if (!m_isLoaded)
    {
        std::cout << WarningMsg << std::endl;
    }

    return m_font;
}

/*static*/ std::atomic<bool> CustomFont::s_isChoosing{false};

} // fonts