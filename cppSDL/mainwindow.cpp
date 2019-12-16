#include "mainwindow.h"
#include "utils.h"
#include "timer.h"
#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include "wall.h"
#include <sstream>
#include <cstdlib>
#include <SDL2/SDL_image.h>
#include <memory>


MainWindow::MainWindow( SDL_Window* window, int windowWidth, int windowHeight )
    :   mWindow( window ),
        mWindowWidth( windowWidth ),
        mWindowHeight( windowHeight ),
        mCellWidth( mWindowWidth / mGridWidth ),
        mCellHeight( mWindowHeight / mGridHeight ),
        mPlayerDiameter( 0.8 * mCellWidth ),
        mEnemyDiameter( 0.8 * mCellWidth )
{
    mRenderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

    Mix_Music* backgroundMusicChunk = Mix_LoadMUS( "Resources/music.wav" );
    Mix_Chunk* soundEffect          = Mix_LoadWAV( "Resources/soundEffect.wav" );

    if( backgroundMusicChunk )
        mMusicList.push_back( backgroundMusicChunk );

    if( soundEffect )
        mSoundList.push_back( soundEffect );

    LoadOverlay();
    LoadLevel();
    LoadPlayers();
    LoadEnemy();
}

MainWindow::~MainWindow()
{
    for( SDL_Texture* texture : mTextureList )
        SDL_DestroyTexture( texture );

    for( Mix_Music* music : mMusicList )
        Mix_FreeMusic( music );

    for( Mix_Chunk* chunk : mSoundList )
        Mix_FreeChunk( chunk );

    for( Entity* entity : mAllEntities )
        delete entity;
}

void MainWindow::Update( double elapsedS, int elapsedMs )
{    
    Timer timer;
    
    std::vector<Event*> eventList;
    for( int i = 0; i < mAllEntities.size(); i++ )
    {
        Entity* entity = mAllEntities.at( i );
        entity->Update( elapsedS, elapsedMs, mAllEntities );

        // Fetch any required events but dont process them until we finish our update pass (incase any events trigger object creation)
        entity->GetPendingEvents( eventList );

        // Erase any dead projectiles
        if( entity->GetType() == Entity::Type::projectile )
        {
            Projectile* projectile = dynamic_cast<Projectile*>( entity );
            if( projectile->IsDead() )
            {
                delete projectile;
                mAllEntities.erase( mAllEntities.begin() + i );
                i--;
            }
        }

        // Erase any dead walls
        else if( entity->GetType() == Entity::Type::destructable_wall )
        {
            Wall* wall = dynamic_cast<Wall*>( entity );
            if( wall->IsDestroyed() )
            {
                double wallX, wallY;
                wall->GetCenter( wallX, wallY );

                int gridX, gridY;
                utils::GridCoordinateFromScreenCoordinate( wallX, wallY, mCellWidth, mCellHeight, gridX, gridY );

                mPathFinder->SetBlockedCell( gridX, gridY, false );

                delete wall;
                mAllEntities.erase( mAllEntities.begin() + i );
                i--;
            }
        }
    }

    ProcessEvents( eventList );

    mFpsRenderer->Update( elapsedS, elapsedMs );

    //SDL_Log( "Update duration: %f seconds", timer.ElapsedS() );
}

void MainWindow::Render()
{
    // Fill the screen blue
    SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( mRenderer );
    SDL_SetRenderDrawBlendMode( mRenderer, SDL_BLENDMODE_BLEND );

    // Render all our textures
    for( SDL_Texture* texture : mTextureList ) {

        SDL_Rect targetRect;
        targetRect.x = 0;
        targetRect.y = 0;
        targetRect.w = 400;
        targetRect.h = 300;
        SDL_RenderCopy( mRenderer, texture, nullptr, &targetRect );
    }

    // Render the grid
    DrawGrid();

    // Render path finding info
    mPathFinder->Render( mRenderer );

    // Render the players
    for( Entity* entity : mAllEntities )
        entity->Render();

    // FPS
    mFpsRenderer->Render( mRenderer );
    
    // Present to the screen
    SDL_RenderPresent( mRenderer );
}

void MainWindow::KeyDownEvent( SDL_KeyboardEvent* event )
{
    // TODO

    if( event->keysym.scancode == SDL_SCANCODE_Q )
    {
        if( !mMusicList.size() )
            return;

        if( Mix_PlayingMusic() )
        {
            if( Mix_PausedMusic() )
            {
                SDL_Log( "Resuming music..." );
                Mix_ResumeMusic();
            }
            else
            {
                SDL_Log( "Pausing music..." );
                Mix_PauseMusic();
            }
        }
        else
        {
            SDL_Log( "Playing music..." );
            Mix_PlayMusic( mMusicList.at( 0 ), -1 );
        }
    }

    else if( event->keysym.scancode == SDL_SCANCODE_E )
    {
        // Play sound effect
        if( !mSoundList.size() )
            return;

        SDL_Log( "Playing sound effect..." );
        Mix_PlayChannel( -1, mSoundList.at( 0 ), 0 );
    }

    mPlayerOne->KeyDownEvent( event );
}

void MainWindow::KeyUpEvent( SDL_KeyboardEvent* event )
{
    mPlayerOne->KeyUpEvent( event );
}

void MainWindow::MouseButtonDownEvent( SDL_MouseButtonEvent* event )
{
    if( !mClickToggle )
    {
        mClickX1 = event->x;
        mClickY1 = event->y;
    }
    
    else
    {
        mClickX2 = event->x;
        mClickY2 = event->y;

        int clickGridX1, clickGridY1;
        utils::GridCoordinateFromScreenCoordinate( mClickX1, mClickY1, mCellWidth, mCellHeight, clickGridX1, clickGridY1 );

        int clickGridX2, clickGridY2;
        utils::GridCoordinateFromScreenCoordinate( mClickX2, mClickY2, mCellWidth, mCellHeight, clickGridX2, clickGridY2 );

        mPathFinder->FindPath( clickGridX1, clickGridY1, clickGridX2, clickGridY2 );
    }

    mClickToggle = !mClickToggle;
}

void MainWindow::ControllerAddedEvent( int controllerID, SDL_ControllerDeviceEvent* event )
{
    // TODO
    SDL_Log( "Controller added (ID: %i)...", controllerID );
}

void MainWindow::ControllerRemovedEvent( int controllerID, SDL_ControllerDeviceEvent* event )
{
    // TODO
    SDL_Log( "Controller removed (ID: %i)...", controllerID );
}

void MainWindow::ControllerAxisMotionEvent( int controllerID, SDL_ControllerAxisEvent* event )
{
    switch( controllerID ) {

        case 0:
        {
            mPlayerOne->ControllerAxisMotionEvent( event );
            break;
        }

        case 1:
        {
            mPlayerTwo->ControllerAxisMotionEvent( event );
            break;
        }

        case 2:
        {
            mPlayerThree->ControllerAxisMotionEvent( event );
            break;
        }

        case 3:
        {
            mPlayerFour->ControllerAxisMotionEvent( event );
            break;
        }

        default: {}
    }
}

void MainWindow::ControllerButtonDownEvent( int controllerID, SDL_ControllerButtonEvent* event )
{
    SDL_Log( "Controller button down (ID: %i, Button ID: %i)...", controllerID, event->button );
    
    switch( controllerID ) {

        case 0:
        {
            mPlayerOne->ControllerButtonDownEvent( event );
            break;
        }

        case 1:
        {
            mPlayerTwo->ControllerButtonDownEvent( event );
            break;
        }

        case 2:
        {
            mPlayerThree->ControllerButtonDownEvent( event );
            break;
        }

        case 3:
        {
            mPlayerFour->ControllerButtonDownEvent( event );
            break;
        }

        default: {}
    }
}

void MainWindow::ControllerButtonUpEvent( int controllerID, SDL_ControllerButtonEvent* event )
{
    SDL_Log( "Controller button up (ID: %i, Button ID: %i)...", controllerID, event->button );

    switch( controllerID ) {

        case 0:
        {
            mPlayerOne->ControllerButtonUpEvent( event );
            break;
        }

        case 1:
        {
            mPlayerTwo->ControllerButtonUpEvent( event );
            break;
        }

        case 2:
        {
            mPlayerThree->ControllerButtonUpEvent( event );
            break;
        }

        case 3:
        {
            mPlayerFour->ControllerButtonUpEvent( event );
            break;
        }

        default: {}
    }
}

void MainWindow::DrawGrid()
{
    SDL_SetRenderDrawColor( mRenderer, 0xEE, 0xEE, 0xEE, SDL_ALPHA_OPAQUE );
    SDL_Point pointA, pointB;

    // Vertical lines
    for( int i = 1; i < mGridWidth; i++ ) {

        pointA.x = i * mCellWidth;
        pointB.x = pointA.x;

        pointA.y = 0;
        pointB.y = mWindowHeight;
        
        SDL_RenderDrawLine( mRenderer, pointA.x, pointA.y, pointB.x, pointB.y );
    }

    // Horizontal lines
    for( int i = 1; i < mGridHeight; i++ ) {

        pointA.x = 0;
        pointB.x = mWindowWidth;

        pointA.y = i * mCellHeight;
        pointB.y = pointA.y;
        
        SDL_RenderDrawLine( mRenderer, pointA.x, pointA.y, pointB.x, pointB.y );
    }
}

void MainWindow::LoadLevel()
{
    std::string texturePath = "Resources/wall.png";
    std::string destructableTexturePath = "Resources/wallDestructable.png";

    // Top
    for( int x = 0; x < mGridWidth; x++ )
    {
        CreateWall( x * mCellWidth, 0, mCellWidth, mCellHeight, texturePath, false );
        mPathFinder->SetBlockedCell( x, 0, true );
    }

    // Bottom
    for( int x = 0; x < mGridWidth; x++ )
    {
        CreateWall( x * mCellWidth, mWindowHeight - mCellHeight, mCellWidth, mCellHeight, texturePath, false );
        mPathFinder->SetBlockedCell( x, mGridHeight - 1, true );
    }

    // Left
    for( int y = 1; y < mGridHeight - 1; y++ )
    {
        CreateWall( 0, y * mCellHeight, mCellWidth, mCellHeight, texturePath, false );
        mPathFinder->SetBlockedCell( 0, y, true );
    }

    // Right
    for( int y = 1; y < mGridHeight - 1; y++ )
    {
        CreateWall( mWindowWidth - mCellWidth, y * mCellHeight, mCellWidth, mCellHeight, texturePath, false );
        mPathFinder->SetBlockedCell( mGridWidth - 1, y, true );
    }

    // Add center pillars
    for( int j = 2; j < mGridHeight - 2; j+=2 )
    {
        for( int i = 2; i < mGridWidth - 2; i+=2 )
        {
            // Don't place on enemy
            if( i == ( mGridWidth / 2 ) && ( j == mGridHeight / 2 ) )
                continue;

            int x = i * mCellWidth;
            int y = j * mCellHeight;

            CreateWall( x, y, mCellWidth, mCellHeight, texturePath, false );
            mPathFinder->SetBlockedCell( i, j, true );
        }
    }

    // Create destructable walls
    for( int i = 1; i < ( mGridWidth - 1 ); i++ )
    {
        for( int j = 1; j < ( mGridHeight - 1 ); j++ )
        {
            // Don't place on players
            if( i == 3 && j == 3 )
                continue;

            if( i == 3 && j == ( mGridHeight - 4 ) )
                continue;

            if( i == ( mGridWidth - 4 ) && j == 3 )
                continue;

            if( i == ( mGridWidth - 4 ) && j == ( mGridWidth - 4 ) )
                continue;

            // Don't place on enemy
            if( i == 10 && j == 10 )
                continue;

            // Only create a wall if either the row or column are odd
            if( !( i % 2 ) && !( j % 2 ) )
                continue;

            if( utils::GetRandomNumber( 0, 100 ) <= mWallCreationChancePercent )
            {
                int x = i * mCellWidth;
                int y = j * mCellHeight;

                CreateWall( x, y, mCellWidth, mCellHeight, destructableTexturePath, true );
                mPathFinder->SetBlockedCell( i, j, true );
            }
        }
    }
}

void MainWindow::LoadPlayers()
{
    { // Player 1

        SDL_Point center; // Top left of the screen
        center.x = ( 3 * mCellWidth ) + ( 0.5 * mCellWidth );
        center.y = ( 3 * mCellHeight ) + ( 0.5 * mCellHeight );

        std::string texturePath = "Resources/playerOne.png";
        std::string deadTexturePath = "Resources/playerOneDead.png";
        std::string stuckTexturePath = "Resources/playerOneStuck.png";

        mPlayerOne = new Player( 1, center, mPlayerDiameter, texturePath, deadTexturePath, stuckTexturePath, mRenderer );
    }

    { // Player 2

        SDL_Point center; // Top right of the screen
        center.x = mWindowWidth - ( 3 * mCellWidth ) - ( 0.5 * mCellWidth );
        center.y = ( 3 * mCellHeight ) + ( 0.5 * mCellHeight );

        std::string texturePath = "Resources/playerTwo.png";
        std::string deadTexturePath = "Resources/playerTwoDead.png";
        std::string stuckTexturePath = "Resources/playerTwoStuck.png";

        mPlayerTwo = new Player( 2, center, mPlayerDiameter, texturePath, deadTexturePath, stuckTexturePath, mRenderer );
    }

    { // Player 3

        SDL_Point center; // Bottom left of the screen
        center.x = ( 3 * mCellWidth ) + ( 0.5 * mCellWidth );
        center.y = mWindowHeight - ( 3 * mCellHeight ) - ( 0.5 * mCellHeight );

        std::string texturePath = "Resources/playerThree.png";
        std::string deadTexturePath = "Resources/playerThreeDead.png";
        std::string stuckTexturePath = "Resources/playerThreeStuck.png";

        mPlayerThree = new Player( 3, center, mPlayerDiameter, texturePath, deadTexturePath, stuckTexturePath, mRenderer );
    }

    { // Player 4

        SDL_Point center; // Bottom right of the screen
        center.x = mWindowWidth - ( 3 * mCellWidth ) - ( 0.5 * mCellWidth );
        center.y = mWindowHeight - ( 3 * mCellHeight ) - ( 0.5 * mCellHeight );

        std::string texturePath = "Resources/playerFour.png";
        std::string deadTexturePath = "Resources/playerFourDead.png";
        std::string stuckTexturePath = "Resources/playerFourStuck.png";

        mPlayerFour = new Player( 4, center, mPlayerDiameter, texturePath, deadTexturePath, stuckTexturePath, mRenderer );
    }

    mAllEntities.push_back( mPlayerOne );
    mAllEntities.push_back( mPlayerTwo );
    mAllEntities.push_back( mPlayerThree );
    mAllEntities.push_back( mPlayerFour );
}

void MainWindow::LoadEnemy()
{
    SDL_Point enemyCenter;
    
    enemyCenter.x = ( 0.5 * mWindowWidth );
    enemyCenter.y = ( 0.5 * mWindowHeight );

    std::vector<Player*> playerList;
    playerList.push_back( mPlayerOne );
    playerList.push_back( mPlayerTwo );
    playerList.push_back( mPlayerThree );
    playerList.push_back( mPlayerFour );

    mEnemy = new Enemy( playerList, enemyCenter, mEnemyDiameter, mRenderer, mWindowWidth, mWindowHeight, mGridWidth, mGridHeight );
    mAllEntities.push_back( mEnemy );
}

void MainWindow::LoadOverlay()
{
    mFpsRenderer    = std::unique_ptr<FpsRenderer>( new FpsRenderer() );
    mPathFinder     = std::unique_ptr<PathFinder>( new PathFinder( mWindowWidth, mWindowHeight, mGridWidth, mGridHeight ) );
}

void MainWindow::CreateWall( int x, int y, int w, int h, std::string texturePath, bool destructable )
{
    SDL_Rect rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    Wall* wall = new Wall( destructable, rect, texturePath, mRenderer );
    mAllEntities.push_back( wall );
}

void MainWindow::CreatePlayerProjectile( int playerID, double x, double y, double xPercent, double yPercent )
{
    SDL_Point center;
    center.x = x;
    center.y = y;

    std::string texturePath = "Resources/projectile.png";

    Projectile* projectile = new Projectile( playerID, center, xPercent, yPercent, mProjectileDiameter, texturePath, mRenderer );
    mAllEntities.push_back( projectile );
}

void MainWindow::ProcessEvents( std::vector<Event*>& eventList )
{
    for( Event* event : eventList )
    {
        switch( event->GetType() )
        {
            case EventType::create_player_projectile:  OnCreatePlayerProjectileEvent( event ); break;

            default: {}
        }

        delete event;
    }
    
    eventList.clear();
}

void MainWindow::OnCreatePlayerProjectileEvent( Event* event )
{
    int     playerID    = event->GetValueInt( EventParameterType::player_id );
    double  x           = event->GetValueDouble( EventParameterType::x );
    double  y           = event->GetValueDouble( EventParameterType::y );
    double  angle       = event->GetValueDouble( EventParameterType::angle );


    // Note: With our coordinate system 0 degrees should be pointing right
    double xPercent, yPercent;
    utils::GetUnitVectorFromAngle( angle, xPercent, yPercent );

    CreatePlayerProjectile( playerID, x, y, xPercent, yPercent );
}
