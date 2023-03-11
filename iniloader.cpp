#include <fstream>
#include <string.h>
#include "includes/inipp.h"

namespace IniLoader {
    inipp::Ini<char> ini;
    std::string fileUrl;
    std::string defaultSection = "default";

    /**
     * @brief Changes the default section of the INI file.
     * 
     * @param sectionParam string - A string that represents the default section of a INI file.
     * 
     * @return void
     * @example
     *      IniLoader::
     */
    void setDefaultSection( std::string sectionParam ) {
        defaultSection = sectionParam;
    }

    /**
     * @brief Loads an INI file.
     * 
     * @param fileUrlParam string - The path to the file to be readed.
     * 
     * @return void
     * @example IniLoader::loadFile("config.ini");
     */
    void loadFile( std::string fileUrlParam ){
        fileUrl = fileUrlParam;
        std::ifstream is( fileUrlParam );
        ini.parse(is);
        ini.strip_trailing_comments(); // Ignore all comments.
        ini.default_section(ini.sections[defaultSection]);
        ini.interpolate();
    }

    /**
     * @brief Gets a value readed from the INI file.
     * 
     * @param section - string - The section where the value is. Something like "default".
     * @param variableName string - The name of the variable to be retrieved inside INI file.
     * @param destinationVar auto& - The variable that will store the value.
     * 
     * @return void
     * @example 
            int version; // Assuming that version is a number variable.
            IniLoader::getValue("config", "version", version);
     */
    void getValue( std::string section, std::string variableName, auto& destinationVar ) {
        inipp::get_value(ini.sections[section], variableName, destinationVar);
    }

};