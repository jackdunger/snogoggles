#include <SFML/Graphics/Rect.hpp>

#include <sstream>
#include <string>
using namespace std;

#include <Viewer/EventInfo.hh>
#include <Viewer/DataStore.hh>
#include <Viewer/RIDS/Event.hh>
#include <Viewer/RIDS/EV.hh>
#include <Viewer/RIDS/MC.hh>
#include <Viewer/GUIProperties.hh>
#include <Viewer/Text.hh>
#include <Viewer/RWWrapper.hh>
using namespace Viewer;
using namespace Frames;

EventInfo::~EventInfo()
{

}

void 
EventInfo::PreInitialise( const ConfigurationTable* configTable )
{
  sf::Rect<double> textSize;
  textSize.Left = 0.0; textSize.Top = 0.0; textSize.Width = 1.0; textSize.Height = 1.0;
  fInfoText = new Text( RectPtr( fRect->NewDaughter( textSize, Rect::eLocal ) ) );
  string hello("Hello");
  fInfoText->SetString( hello );
  fInfoText->SetColour( GUIProperties::GetInstance().GetGUIColourPalette().GetB( eBase ) );
}

void 
EventInfo::EventLoop()
{
  while( !fEvents.empty() )
    {
      fEvents.pop();
    }
}
void 
EventInfo::Render2d( RWWrapper& renderApp,
		     const RenderState& renderState )
{
  stringstream eventInfo;
  eventInfo.precision( 0 );
  eventInfo << fixed;

  const RIDS::Event& event = DataStore::GetInstance().GetCurrentEvent();
  if( event.ExistEV() )
    {
      RIDS::EV& ev = event.GetEV();
      eventInfo << "EV:" << endl;
      eventInfo << "\tGTID: " << ev.GetGTID() << endl;
      eventInfo << "\tTrigger Word: " << ToHexString( ev.GetTriggerWord() ) << endl;
      eventInfo << "\tNhit (Cal):" << ev.GetCalNHits() << endl;
      eventInfo << "\tNhit (UnCal):" << ev.GetUnCalNHits() << endl;
      eventInfo << "\tNhit (Truth):" << ev.GetTruthNHits() << endl;
    }
  if( event.ExistMC() )
    {
      RIDS::MC& mc = event.GetMC();
      eventInfo << "MC:" << endl;
      eventInfo << "\tNhit:" << mc.GetNHits() << endl;
    }
  fInfoText->SetString( eventInfo.str() );
  fInfoText->SetColour( GUIProperties::GetInstance().GetGUIColourPalette().GetText() );
  renderApp.Draw( *fInfoText );  
}

string
EventInfo::ToByteString( int number )
{
  stringstream byteStream;
  for( size_t iLoop = 1; iLoop <= sizeof(int) * 8; iLoop++ )
    {    
      const int val = number >> ( sizeof(int) * 8 - iLoop );
      if( val & 1 )
	byteStream << "1";
      else
	byteStream << "0";
      if( iLoop % 4 == 0 )
	byteStream << " ";
    }
  return byteStream.str();
}

string
EventInfo::ToHexString( int number )
{
  stringstream hexStream;
  hexStream << hex << uppercase;
  hexStream << "0x" << number;
  return hexStream.str();
}
