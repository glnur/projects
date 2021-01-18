#ifndef CUSTOM_FONT_HEADER
#define CUSTOM_FONT_HEADER

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace fonts
{

class CustomFont
{
public:
    CustomFont();

    std::string ChooseManually();
    bool Load(const std::string fontName);
    void WriteMessage(const std::string& message);

protected:
    void ShowAllFontsExamples() noexcept;
    const size_t& GetLength() const;
    const std::string& GetFont() const;

protected:
    std::vector<std::string> m_fontList;

private:
    unsigned m_length;
    std::string m_font;
    std::string m_xmlFile;
    boost::property_tree::ptree m_tree;
    bool m_isLoaded{false};
};

} // fonst

#endif // CUSTOM_FONT_HEADER
