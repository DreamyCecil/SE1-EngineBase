/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_NETWORK_H
#define SE_INCL_NETWORK_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/FileName.h>
#include <Engine/Base/Timer.h>
#include <Engine/Base/Stream.h>
#include <Engine/World/World.h>
#include <Engine/Network/MessageDispatcher.h>
#include <Engine/Templates/StaticArray.h>

#define NET_MAXGAMECOMPUTERS SERVER_CLIENTS // max overall computers in game
#define NET_MAXGAMEPLAYERS 16 // max overall players in game
#define NET_MAXLOCALPLAYERS 4 // max players on a single machine

#define NET_WAITMESSAGE_DELAY 50 // wait time between receive message attempts

#define NET_MAXSESSIONPROPERTIES 2048 // size of buffer for custom use by CGame and entities

// Timer handler for network timer loop
class ENGINE_API CNetworkTimerHandler : public CTimerHandler {
  public:
    // This is called every TickQuantum seconds
    virtual void HandleTimer(void);
};

// Demo synchronization constants
#define DEMOSYNC_REALTIME (0.0f)
#define DEMOSYNC_STOP     (-1.0f)

enum NetGraphEntryType {
  NGET_ACTION = 0,
  NGET_NONACTION,
  NGET_MISSING,
  NGET_SKIPPEDACTION,
  NGET_REPLICATEDACTION,
};

struct NetGraphEntry {
  enum NetGraphEntryType nge_ngetType; // type of packet/event
  FLOAT nge_fLatency; // latency in seconds
  void Clear(void);
};

// Network session description
class ENGINE_API CNetworkSession {
  public:
    CListNode ns_lnNode; // for linking in list of available sessions
    CTString ns_strAddress; // session address

    CTString ns_strSession; // session name
    CTString ns_strWorld; // world name

    TIME ns_tmPing; // current players
    INDEX ns_ctPlayers; // current players
    INDEX ns_ctMaxPlayers; // max number of players

    CTString ns_strGameType; // game type
    CTString ns_strMod; // active mod
    CTString ns_strVer; // version

  public:
    // Constructor
    CNetworkSession(void);

    // Construct a session for connecting to certain server
    CNetworkSession(const CTString &strAddress);

    void Copy(const CNetworkSession &nsOriginal);
};

// Game, used for general game initialization/manipulation
class ENGINE_API CNetworkLibrary : public CMessageDispatcher {
  public:
    BOOL ga_IsServer; // set if this is a server computer
    CServer &ga_srvServer; // server (active only if this is server computer)

    CTCriticalSection ga_csNetwork; // critical section for access to network data

    CSessionState &ga_sesSessionState;          // local session state
    CStaticArray<CPlayerSource> ga_aplsPlayers; // local players
    CTString ga_strSessionName;                 // name of current session
    CTString ga_strServerAddress;               // address of game server (if joined)
    INDEX ga_ulDemoMinorVersion;                // minor version of build that created this demo
    CTFileName ga_fnmWorld;                     // filename of current world
    UBYTE *ga_pubDefaultState;                  // default state for connecting (server only)
    SLONG ga_slDefaultStateSize;
    UBYTE ga_aubDefaultProperties[NET_MAXSESSIONPROPERTIES];
    UBYTE *ga_pubCRCList; // list of files for CRC checking (server only)
    SLONG ga_slCRCList;
    ULONG ga_ulCRC; // CRC of CRCs of all files in the list

    BOOL ga_bLocalPause;                    // local pause for single player/demo
    BOOL ga_bDemoRec;                       // set if currently recording a demo
    CTFileStream ga_strmDemoRec;            // currently recorded demo file
    BOOL ga_bDemoPlay;                      // set if currently playing a demo
    BOOL ga_bDemoPlayFinished;              // set if currently playing demo has finished
    CTFileStream ga_strmDemoPlay;           // currently played demo file
    CTimerValue ga_tvDemoTimerLastTime;     // real time timer for demo synchronization
    CNetworkTimerHandler ga_thTimerHandler; // handler for driving the timer loop
    INDEX ga_ctTimersPending;               // number of timer loops pending

    CTFileName ga_fnmNextLevel;  // world for next level
    BOOL ga_bNextRemember;       // remember old levels when changing to new one
    INDEX ga_iNextLevelUserData; // user data for next level

    CListHead ga_lhEnumeratedSessions; // list of sessions found after last enumeration was triggered
    FLOAT ga_fEnumerationProgress;     // enumeration progress percentage (0-1)
    CTString ga_strEnumerationStatus;  // description of current operation
    BOOL ga_bEnumerationChange;        // this is raised if something was changed in the list

    CTString ga_strRequiredMod; // set if connection failed due to a different mod running on the server

    UBYTE ga_aubProperties[NET_MAXSESSIONPROPERTIES]; // Buffer for custom use by CGame and entities

    BOOL IsServer(void) {
      return ga_IsServer;
    };

    // Make actions packet for local players and send to server
    void SendActionsToServer(void);

    // Loop executed in timer interrupt, every game tick
    void TimerLoop(void);

    // Add the timer handler
    void AddTimerHandler(void);

    // Remove the timer handler
    void RemoveTimerHandler(void);

    // Really do the level change
    void ChangeLevel_internal(void);

    // Save current version of engine
    void WriteVersion_t(CTStream &strm);

    // Load version of engine saved in file and check against current
    void CheckVersion_t(CTStream &strm, BOOL bAllowReinit, BOOL &bNeedsReinit);

    // Add a value to the netgraph
    void AddNetGraphValue(enum NetGraphEntryType nget, FLOAT fLatency);

    // Call this while game is not running - to update server enumeration lists
    void GameInactive(void);

    // Automatically adjust network settings
    void AutoAdjustSettings(void);

    // Make default state data for creating deltas
    void MakeDefaultState(const CTFileName &fnmWorld, ULONG ulSpawnFlags, void *pvSessionProperties);

    // Initialize gathering of file CRCs to CRC table
    void InitCRCGather(void);

    // Finish gathering of file CRCs to CRC table (call for server only!)
    void FinishCRCGather(void);

  public:
    CWorld ga_World; // local copy of world
    FLOAT ga_fDemoTimer; // timer for demo playback (in seconds)
    FLOAT ga_fDemoRealTimeFactor; // slow/fast playback factor (for DEMOSYNC_REALTIME only)
    FLOAT ga_fGameRealTimeFactor; // game time accelerator
    FLOAT ga_fDemoSyncRate; // demo sync speed in FPS (or realtime/stop)
    CStaticArray<struct NetGraphEntry> ga_angeNetGraph; // array of netgraph entries

    // Constructor
    CNetworkLibrary(void);

    // Destructor
    ~CNetworkLibrary(void);

    DECLARE_NOCOPYING(CNetworkLibrary);

    // Initialize game management
    void Init(const CTString &strGameID);

    // Start a peer-to-peer game session
    void StartPeerToPeer_t(const CTString &strSessionName, const CTFileName &fnmWorld, ULONG ulSpawnFlags, INDEX ctMaxPlayers,
                           BOOL bWaitAllPlayers,
                           void *pvSessionProperties); // throw char *

    // Trigger sessions enumeration over LAN and iNet
    void EnumSessions(BOOL bInternet);

    // Join a running multi-player game
    void JoinSession_t(const CNetworkSession &nsSesssion, INDEX ctLocalPlayers); // throw char *

    // Start playing a demo
    void StartDemoPlay_t(const CTFileName &fnDemo); // throw char *

    // Test if currently playing a demo
    BOOL IsPlayingDemo(void);

    // Test if currently recording a demo
    BOOL IsRecordingDemo(void);

    // Test if currently playing demo has finished
    BOOL IsDemoPlayFinished(void);

    // Stop currently running game
    void StopGame(void);

    // Pause/unpause the game
    void TogglePause(void);

    // Check if game is paused
    BOOL IsPaused(void);

    // Check if game is waiting for more players to connect
    BOOL IsWaitingForPlayers(void);

    // Check if game is waiting for server
    BOOL IsWaitingForServer(void);

    // Mark that the game has finished - called from AI
    void SetGameFinished(void);
    BOOL IsGameFinished(void);

    // Manipulation with realtime factor for slower/faster time - called from AI
    void SetRealTimeFactor(FLOAT fSpeed);
    FLOAT GetRealTimeFactor(void);

    // Check if having connnection problems (not getting messages from server regulary)
    BOOL IsConnectionStable(void);

    // Check if completely disconnected and why
    BOOL IsDisconnected(void);
    const CTString &WhyDisconnected(void);

    // Server side pause (for single player or demo only)
    void SetLocalPause(BOOL bPause);
    BOOL GetLocalPause(void);

    // Get server/client name and address
    void GetHostName(CTString &strName, CTString &strAddress);

    // Check if playing in network or locally
    BOOL IsNetworkEnabled(void);

    // Check if game session is currently doing prediction
    BOOL IsPredicting(void);

    // Initiate level change
    void ChangeLevel(const CTFileName &fnmNextLevel, BOOL bRemember, INDEX iUserData);

    // Obtain file name of world that is currently loaded
    CTFileName &GetCurrentWorld(void) {
      return ga_fnmWorld;
    };

    // Start recording a demo
    void StartDemoRec_t(const CTFileName &fnDemo); // throw char *

    // Stop recording a demo
    void StopDemoRec(void);

    // Read current game situation from a stream
    void Read_t(CTStream *pstr); // throw char *

    // Write current game situation into a stream
    void Write_t(CTStream *pstr); // throw char *

    // Save the game
    void Save_t(const CTFileName &fnmGame); // throw char *

    // Load the game
    void Load_t(const CTFileName &fnmGame); // throw char *

    // Save a debugging game
    void DebugSave(void); // this doesn't throw anything

    // Add a new player to game
    CPlayerSource *AddPlayer_t(CPlayerCharacter &pcCharacter); // throw char *

    // Loop executed in main application thread
    void MainLoop(void);

    // Get player entity for a given local player
    CEntity *GetLocalPlayerEntity(CPlayerSource *ppls);

    // Get player entity for a given player by name
    CEntity *GetPlayerEntityByName(const CTString &strName);

    // [Cecil] TODO: Remake and optimize these functions; add functions for iteration through classes

    // Get number of entities with given name
    INDEX GetNumberOfEntitiesWithName(const CTString &strName);

    // Get n-th entity with given name
    CEntity *GetEntityWithName(const CTString &strName, INDEX iEntityWithThatName);

    // Check if a given player is local to this computer
    BOOL IsPlayerLocal(CEntity *pen);

    // Get player source for a given player if it is local to this computer
    CPlayerSource *GetPlayerSource(CEntity *pen);

    // Get game time in currently running game
    TICK NetworkGameTime(void);

    // Get session properties for current game.
    void *GetSessionProperties(void);

    // Send chat message from some players to some other players.
    void SendChat(ULONG ulFrom, ULONG ulTo, const CTString &strMessage);
};

// Make default state for a network game
extern void NET_MakeDefaultState_t(const CTFileName &fnmWorld, ULONG ulSpawnFlags, void *pvSessionProperties,
                                   CTStream &strmState); // throw char *

// Pointer to global instance of the only network object in the application
ENGINE_API extern CNetworkLibrary *_pNetwork;

// Convert string address to a number
ENGINE_API extern ULONG StringToAddress(const CTString &strAddress);

// Convert address to a printable string
ENGINE_API extern CTString AddressToString(ULONG ulHost);

#endif /* include-once check. */
