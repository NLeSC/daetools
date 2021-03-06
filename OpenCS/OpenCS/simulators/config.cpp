/***********************************************************************************
                 OpenCS Project: www.daetools.com
                 Copyright (C) Dragan Nikolic
************************************************************************************
OpenCS is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License version 3 as published by the Free Software
Foundation. OpenCS is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with
the OpenCS software; if not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/
#include <sstream>
#include "daesimulator.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/detail/file_parser_error.hpp>

namespace cs_dae_simulator
{
daeSimulationOptions::daeSimulationOptions()
{
    ptree = new boost::property_tree::ptree();
}

daeSimulationOptions::~daeSimulationOptions()
{
    boost::property_tree::ptree* pt = (boost::property_tree::ptree*)ptree;

    delete pt;
}

daeSimulationOptions& daeSimulationOptions::GetConfig()
{
    static daeSimulationOptions cfg;
    return cfg;
}

void daeSimulationOptions::Load(const std::string& jsonFilePath)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    pt.clear();
    boost::property_tree::json_parser::read_json(jsonFilePath, pt);
    configFile = jsonFilePath;
}

void daeSimulationOptions::LoadString(const std::string& jsonOptions)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    pt.clear();
    std::stringstream ss(jsonOptions);
    boost::property_tree::json_parser::read_json(ss, pt);
    configFile.clear();
}

bool daeSimulationOptions::HasKey(const std::string& strPropertyPath) const
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    try
    {
        pt.get_child(strPropertyPath);
        return true;
    }
    catch(boost::property_tree::ptree_bad_path& e)
    {
    }

    return false;
}

std::string daeSimulationOptions::ToString(void) const
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    return ss.str();
}

bool daeSimulationOptions::GetBoolean(const std::string& strPropertyPath)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<bool>(strPropertyPath);
}

double daeSimulationOptions::GetFloat(const std::string& strPropertyPath)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<double>(strPropertyPath);
}

int daeSimulationOptions::GetInteger(const std::string& strPropertyPath)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<int>(strPropertyPath);
}

std::string daeSimulationOptions::GetString(const std::string& strPropertyPath)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<std::string>(strPropertyPath);
}

bool daeSimulationOptions::GetBoolean(const std::string& strPropertyPath, const bool defValue)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<bool>(strPropertyPath, defValue);
}

double daeSimulationOptions::GetFloat(const std::string& strPropertyPath, const double defValue)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<double>(strPropertyPath, defValue);
}

int daeSimulationOptions::GetInteger(const std::string& strPropertyPath, const int defValue)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<int>(strPropertyPath, defValue);
}

std::string daeSimulationOptions::GetString(const std::string& strPropertyPath, const std::string& defValue)
{
    boost::property_tree::ptree& pt = *((boost::property_tree::ptree*)ptree);

    return pt.get<std::string>(strPropertyPath, defValue);
}

}
