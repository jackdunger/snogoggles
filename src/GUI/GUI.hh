////////////////////////////////////////////////////////////////////////
/// \class Viewer::GUI
///
/// \brief   All GUI objects must derive from this
///
/// \author  Phil Jones <p.g.jones@qmul.ac.uk>
///
/// REVISION HISTORY:\n
///     29/06/11 : P.Jones - First Revision, new file. \n
///     20/02/12 : P.Jones - Second Revision, uses new Rect object. \n
///
/// \detail  Base class for GUI objects, handles visibility position 
///          and ID aspects.
///
////////////////////////////////////////////////////////////////////////

#ifndef __Viewer_GUI__
#define __Viewer_GUI__

#include <SFML/Graphics/Rect.hpp>

#include <Viewer/GUIEvent.hh>
#include <Viewer/RectPtr.hh>

namespace sf
{
  class RenderWindow;
}

namespace Viewer
{
  class Event;
  class RWWrapper;

  enum EGUIState { eBase, eHighlight, eActive };

class GUI
{
public:
  /// Constructor, must pass GUI position rect and an ID
  GUI( RectPtr rect,
       unsigned int guiID );

  virtual ~GUI();
  /// Get the unique GUI ID within the frame (not globally unique)
  inline unsigned int GetGUIID() const;
  /// Get the globally unique GUI ID
  inline unsigned int GetGlobalID() const;
  /// Get the Rect
  inline RectPtr GetRect();
  /// Set the GUI as hidden or not
  inline void SetHidden( bool hidden = true );
  /// Query if GUI is hidden
  inline bool Hidden();
  /// Change the local guiID
  inline void SetGUIID( unsigned int guiID );
  /// Check if local coord point lies within the GUI object
  inline bool ContainsPoint( const sf::Vector2<double>& localPoint );
  /// Pure virtual render call, local render window
  virtual void Render( RWWrapper& renderApp ) = 0;
  /// Event handler
  virtual GUIEvent NewEvent( const Event& event ) = 0;  

  static int fsKeyboardFocus; /// < If greater than zero then key strokes go to one GUI
protected:
  RectPtr fRect; /// < Position rect
  unsigned int fID;       /// < Local ID
  unsigned int fGlobalID; /// < Global ID
  bool fHidden;           /// < Is GUI hidden
private:
  static unsigned int fsNextID; /// < Global GUI counter
};

inline
GUI::GUI( RectPtr rect,
	  unsigned int guiID )
  : fRect( rect ), fID( guiID )
{
  fGlobalID = fsNextID++;
  fHidden = false;
}

inline unsigned int 
GUI::GetGUIID() const
{
  return fID;
}

inline unsigned int 
GUI::GetGlobalID() const
{
  return fGlobalID;
}

inline RectPtr 
GUI::GetRect()
{
  return fRect;
}

inline void
GUI::SetHidden( bool hidden )
{
  fHidden = hidden;
}

inline bool
GUI::Hidden()
{
  return fHidden;
}

inline void 
GUI::SetGUIID( unsigned int guiID )
{
  fID = guiID;
}

inline bool 
GUI::ContainsPoint( const sf::Vector2<double>& point )
{
  return fRect->ContainsPoint( point, Rect::eResolution );
}

} // ::Viewer

#endif
