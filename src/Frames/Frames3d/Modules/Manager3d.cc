#include <RAT/DS/Run.hh>

#include <Viewer/Manager3d.hh>
#include <Viewer/CameraManager3d.hh>
#include <Viewer/HitManager3d.hh>
#include <Viewer/TrackManager3d.hh>
#include <Viewer/GeoManager3d.hh>
#include <Viewer/FitterManager3d.hh>
#include <Viewer/StringUtils.hh>
#include <Viewer/ModuleFactory3d.hh>
#include <Viewer/ConfigurationTable.hh>
#include <Viewer/EventData.hh>
#include <Viewer/Axes3d.hh>
#include <Viewer/ConfigTableUtils.hh>
#include <Viewer/CheckBoxLabel.hh>
#include <Viewer/GUIManager.hh>

#include <vector>
#include <string>

namespace RAT {
    namespace DS {
        class EV;
        class PMTProperties;
        class MC;
    }; // namespace DS
}; // namespace RAT

namespace Viewer {
namespace Frames {

const std::string Manager3d::DISPLAY_AXIS_TAG = "DisplayAxes";

Manager3d::Manager3d( const std::string& options )
{
    fDisplayAxis = true;
    fInitialised = false;
    fCheckBoxLabel = NULL;

    std::vector<std::string> tokens;
    StringUtils::SplitString( options, " ", tokens );
    ModuleFactory3d::GetInstance()->SetAllModules( this, tokens );
}

Manager3d::~Manager3d()
{
    DeleteAllModules();
}

void Manager3d::SetAllModules( CameraManager3d* camera, HitManager3d* hits, TrackManager3d* tracks, GeoManager3d* geo, FitterManager3d* fitter)
{
    fCameraManager = camera;
    fHitManager = hits;
    fTrackManager = tracks;
    fGeoManager = geo;
    fFitterManager = fitter;

    if( fCameraManager != NULL )
        fAxes = new Axes3d( fCameraManager->SuggestedAxisLength() );
}

void Manager3d::DeleteAllModules()
{
    delete fCameraManager;    fCameraManager = NULL; 
    delete fHitManager;       fHitManager = NULL; 
    delete fTrackManager;     fTrackManager = NULL;
    delete fGeoManager;       fGeoManager = NULL;
    delete fFitterManager;    fFitterManager = NULL;

    delete fAxes;             fAxes = NULL;
}

void Manager3d::LateInitialise()
{
    if( fInitialised == false )
    {
        // checks that the camera manager is not null
        // if the camera manager is null, throws an error
        if( fCameraManager == NULL )
            throw NoCameraError();

        GeoManager3d::LoadFileSafe( fGeoManager );
        CreateGUIObjects();
        fViewportArea = GetViewportRect();
        fInitialised = true;
    }
}

void Manager3d::LoadWithoutSettingModules( ConfigurationTable& configTable )
{
    LoadModuleConfigurations( configTable );
    ConfigTableUtils::GetBooleanSafe( &configTable, DISPLAY_AXIS_TAG, fDisplayAxis );
}

void Manager3d::LoadModuleConfigurations( ConfigurationTable& configTable )
{
    Module3d::LoadConfigurationSafe( fCameraManager, configTable );
    Module3d::LoadConfigurationSafe( fHitManager, configTable );
    Module3d::LoadConfigurationSafe( fTrackManager, configTable );
    Module3d::LoadConfigurationSafe( fGeoManager, configTable );
    Module3d::LoadConfigurationSafe( fFitterManager, configTable );
}

void Manager3d::SaveConfiguration( ConfigurationTable& configTable ) 
{
    ConfigTableUtils::SetBoolean( &configTable, DISPLAY_AXIS_TAG, fDisplayAxis );
    SaveModuleConfigurations( configTable );
}

void Manager3d::SaveModuleConfigurations( ConfigurationTable& configTable ) 
{
    Module3d::SaveConfigurationSafe( fCameraManager, configTable );
    Module3d::SaveConfigurationSafe( fHitManager, configTable );
    Module3d::SaveConfigurationSafe( fTrackManager, configTable );
    Module3d::SaveConfigurationSafe( fGeoManager, configTable );
    Module3d::SaveConfigurationSafe( fFitterManager, configTable );
}

void Manager3d::EventLoop() 
{
    LateInitialise();

    while( !fEvents.empty() )
        fEvents.pop();

    if( fCheckBoxLabel != NULL )
        fDisplayAxis = fCheckBoxLabel->GetState();

    Module3d::EventLoopSafe( fCameraManager );
    Module3d::EventLoopSafe( fHitManager );
    Module3d::EventLoopSafe( fTrackManager );
    Module3d::EventLoopSafe( fGeoManager );
    Module3d::EventLoopSafe( fFitterManager );
}

void Manager3d::Render2d( RWWrapper& windowApp )
{

}

void Manager3d::Render3d( ) 
{
    LateInitialise();

    RAT::DS::EV* ev = EventData::GetInstance().GetCurrentEV();
    RAT::DS::MC* mc = EventData::GetInstance().GetCurrentMC();
    RAT::DS::PMTProperties* pmtList = EventData::GetInstance().GetRun()->GetPMTProp();

    Rect viewportRect;
    viewportRect.SetFromLocalRect( fViewportArea, fFrameRect );

    fCameraManager->SetUpCameraSystem( viewportRect.GetViewport() ); 
    HitManager3d::RenderHitsSafe( fHitManager, ev, pmtList );
    TrackManager3d::RenderTracksSafe( fTrackManager, mc );
    GeoManager3d::RenderGeometrySafe( fGeoManager );
    FitterManager3d::RenderFitVertexSafe( fFitterManager );

    if( fDisplayAxis == true )
        Axes3d::RenderAxesSafe( fAxes );
}

void Manager3d::CreateAxesGUIObject( sf::Rect< double > rect )
{
    fCheckBoxLabel = fGUIManager.NewGUI< GUIs::CheckBoxLabel >( rect );
    fCheckBoxLabel->SetLabel( "Display Axes" );
    fCheckBoxLabel->SetState( fDisplayAxis );
}

}; // namespace Frames
}; // namespace Viewer
