#include "../Gamee/GMessageBox.h"
#include "types.h"
#include <iostream>
#include "../Core/rapidxml/rapidxml.hpp"
#include "../Core/rapidxml/rapidxml_utils.hpp"

bool Parse(rapidxml::xml_document<> &doc, std::vector<char> &buffer, const char *fileName)
{
    char buff[4000];
    buff[0] = 0;
    try
        {
            doc.parse<0>(buffer.data());
        }
        catch (const std::runtime_error& e)
        {
            sprintf(buff, "%s Runtime error was: %s", fileName, e.what());
        }
        catch (const rapidxml::parse_error& e)
        {
            sprintf(buff, "%s Parse error was: %s", fileName, e.what());
        }
        catch (const std::exception& e)
        {
            sprintf(buff, "%s Error was: %s", fileName, e.what());
        }
        catch (...)
        {
            sprintf(buff, "%s An unknown error occurred.", fileName);
        }

    if (buff[0])
    {
        GMessageBoxShow(buff);
        return false;
    }
    else
        return true;
}

