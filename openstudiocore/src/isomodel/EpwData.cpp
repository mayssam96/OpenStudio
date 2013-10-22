#include <isomodel/EpwData.hpp>
#include "SolarRadiation.hpp"

namespace openstudio {
namespace isomodel {

EpwData::EpwData(const openstudio::path &t_path)
  : m_data(7, std::vector<double>(8760))
{
  loadData(t_path);
}

void EpwData::parseHeader(const std::string &line)
{
  std::stringstream linestream(line);

  //cout << "Weather Location Header: "<<endl;
  for(int i = 0;i<10;i++)
  {
    std::string s;
    std::getline(linestream, s, ',');
    switch(i)
    {	
      case 1:
        m_location = s;
        //cout << "\tLocation: " << s <<endl;
        break;
      case 5:
        m_stationid = s;
        //cout << "\tStation ID: " << s <<endl;
        break;
      case 6:
        m_latitude = atof(s.c_str());
        //cout << "\tLatitude: " << s <<endl;
        break;
      case 7:
        m_longitude = atof(s.c_str());
        //cout << "\tLongitude: " << s <<endl;
        break;
      case 8:
        m_timezone = atoi(s.c_str());
        //cout << "\tTimezone: " << s <<endl;
        break;
      default:
        break;
    }
  }			
}

void EpwData::parseData(const std::string &line, size_t row)
{
  std::stringstream linestream(line);
  size_t col = 0;

  for(size_t i = 0;i<22;i++)
  {
    std::string s;
    std::getline(linestream, s, ',');
    switch(i)
    {	
      case 6:
      case 7:
      case 8:
      case 13:
      case 14:
      case 15:
      case 21:
        assert(col < m_data.size());
        assert(row < m_data[col].size());
        m_data[col++][row] = ::atof(s.c_str());
        break;
      default:
        break;
    }
  }	
}

std::string EpwData::toISOData() const {
  TimeFrame frames;
  SolarRadiation pos(frames, *this);
  pos.Calculate();
  std::stringstream sstream;
  sstream << "mdbt" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i << "," << pos.monthlyDryBulbTemp()[i] <<std::endl;
  }
  sstream << "mwind" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i << "," << pos.monthlyWindspeed()[i] <<std::endl;
  }
  sstream << "mEgh" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i << "," << pos.monthlyGlobalHorizontalRadiation()[i] <<std::endl;
  }
  sstream << "hdbt" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i;
    for(int h = 0;h<24;h++)
    {
      sstream << "," << pos.hourlyDryBulbTemp()[i][h];
    } 
    sstream <<std::endl;
  }
  sstream << "hEgh" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i;
    for(int h = 0;h<24;h++)
    {
      sstream << "," << pos.hourlyGlobalHorizontalRadiation()[i][h];
    } 
    sstream <<std::endl;
  }
  sstream << "solar" << std::endl;
  for(int i = 0;i<12;i++)
  {
    sstream << i;
    for(int s = 0;s<SolarRadiation::NUM_SURFACES;s++)
    {
      sstream << "," << pos.monthlySolarRadiation()[i][s];
    } 
    sstream <<std::endl;
  }
  return sstream.str();
}

void EpwData::loadData(const openstudio::path &fn)
{
  // Array was fully initialized in constructor
  std::ifstream myfile(openstudio::toString(fn).c_str());
  if (myfile.is_open())
  {
    size_t i = 0;
    size_t row = 0;
    while ( myfile.good() && row < 8760)
    {
      i++;
      std::string line;
      getline(myfile,line);
      if(i==1)
      {
        parseHeader(line);
      }
      else if(i > 8)
      {
        parseData(line,row++);
      }      
    }
    myfile.close();
  } else {
    throw std::runtime_error("Unable to open weather file: " + openstudio::toString(fn));
  }
}
}
}
