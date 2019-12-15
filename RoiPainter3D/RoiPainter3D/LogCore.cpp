#include "stdafx.h"
#include "LogCore.h"
#include "time.h"

using namespace std;

LogCore::LogCore()
{
  m_b_recording = false;
  m_lines.clear();
}



void LogCore::StartLogger()
{
  m_b_recording = true;
  m_lines.clear();
  Add("StartLogging");
}


void LogCore::Add( std::string s)
{
  if(!m_b_recording ) return;

  time_t currenttime = time(NULL);
  struct tm *t = localtime(&currenttime);
  string log = 
      to_string( t->tm_year+1900 )
    + to_string( t->tm_mon + 1   )
    + string((t->tm_mday < 10 ) ? "0" : "") + to_string( t->tm_mday )
    + string((t->tm_hour < 10 ) ? "0" : "") + to_string( t->tm_hour )
    + string((t->tm_min  < 10 ) ? "0" : "") + to_string( t->tm_min  )
    + string((t->tm_sec  < 10 ) ? "0" : "") + to_string( t->tm_sec  );
  m_lines.push_back(std::make_pair(log,s));
}



void LogCore::CloseLogger(std::string filename)
{
  if(!m_b_recording ) return;

  //export 
  std::ofstream ofs( filename.c_str() );

  if (!ofs){
    std::cout << "failed to open the file " << filename.c_str() << "\n";
    return;
  }

  ofs << "TIME, MESSAGE\n";
 
  for ( auto line : m_lines) 
    ofs << line.first.c_str() << ", " << line.second.c_str() << "\n";

  ofs.close();


  m_b_recording = false;
}

