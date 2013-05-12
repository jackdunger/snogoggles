#include <SFML/Window/Event.hpp>

#include <Viewer/GUIManager.hh>
#include <Viewer/Event.hh>
#include <Viewer/GUI.hh>
using namespace Viewer;

GUIManager::~GUIManager()
{
  Clear();
}

void
GUIManager::Clear()
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    delete fGUIObjects[uGUI];
  fGUIObjects.clear();
}

GUIEvent 
GUIManager::NewEvent( const Event& event )
{
  GUIEvent retEvent; // Returned event
  int oldFocus = fFocus;
  switch( event.type )
    {
    case sf::Event::LostFocus:
      for( vector<GUI*>::iterator iTer = fGUIObjects.begin(); iTer != fGUIObjects.end(); iTer++ )
        (*iTer)->NewEvent( event );
      fFocus = -1;
      break;
// First events that go straight through to Focus
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
        retEvent = fGUIObjects[fFocus]->NewEvent( event );
      break;
// Now events that go straight through to Focus and change the focus   
    case sf::Event::MouseButtonReleased:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
        retEvent = fGUIObjects[fFocus]->NewEvent( event );
      fFocus = FindGUI( event.GetPos() );
      break;
// Now events that change the focus   
    case sf::Event::MouseMoved:
    case sf::Event::MouseButtonPressed:
      fFocus = FindGUI( event.GetPos() );
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
        retEvent = fGUIObjects[fFocus]->NewEvent( event );
      break;

// All other events, e.g. text
    default:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
        retEvent = fGUIObjects[fFocus]->NewEvent( event );
      break;
    }
// Focus change
  if( oldFocus != fFocus && oldFocus != -1 ) 
    {
      Event lostFocus( sf::Event::LostFocus );
      fGUIObjects[oldFocus]->NewEvent( lostFocus );
    }
  return retEvent;
}
  
void 
GUIManager::Render( RWWrapper& windowApp )
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    if( !fGUIObjects[uGUI]->Hidden() )
      fGUIObjects[uGUI]->Render( windowApp );
}
  

GUI* 
GUIManager::GetGUI( unsigned int guiID ) const
{
  return fGUIObjects[guiID];
}

void
GUIManager::DeleteGUI( unsigned int guiID ) 
{
  for( vector<GUI*>::iterator iTer = fGUIObjects.begin(); iTer != fGUIObjects.end(); iTer++ )
    if( (*iTer)->GetGUIID() == guiID )
      {
	delete *iTer;
	fGUIObjects.erase( iTer );
      }
}

int
GUIManager::FindGUI( sf::Vector2<double> point )
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    if( !fGUIObjects[uGUI]->Hidden() && fGUIObjects[uGUI]->ContainsPoint( point ) )
      return uGUI;
  return -1;
}
