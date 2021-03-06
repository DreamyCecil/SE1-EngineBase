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

#ifndef SE_INCL_ANIMOBJECT_H
#define SE_INCL_ANIMOBJECT_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Anim/AnimData.h>

// An instance of animatable object
#define AOF_PAUSED       (1L << 0) // current animation is paused
#define AOF_LOOPING      (1L << 1) // anim object is playing a looping animation
#define AOF_NORESTART    (1L << 2) // don't restart anim (used for PlayAnim())
#define AOF_SMOOTHCHANGE (1L << 3) // smoothly change between anims

class CAnimObject : public CChangeable {
  public:
    TICK ao_llAnimStart; // time when current anim was started
    INDEX ao_iCurrentAnim; // index of active animation
    ULONG ao_ulFlags; // flags
    INDEX ao_iLastAnim; // index of last animation (for smooth transition)

    // Calculate frame that coresponds to given time
    INDEX FrameInTime(TIME time) const;

  public:
    CAnimData *ao_AnimData;

  public:
    // Some of usual smart pointer functions are implemented
    // because CAnimObjects behave like smart pointers to CAnimData objects

    // Constructor
    ENGINE_API CAnimObject(void);
    // Destructor
    ENGINE_API ~CAnimObject(void);

    // Copy from another object of same class
    ENGINE_API void Copy(CAnimObject &aoOther);

    // Synchronize with another animation object (set same anim and frames)
    ENGINE_API void Synchronize(CAnimObject &aoOther);

    // Copying of AnimObjects is not allowed
    inline CAnimObject(const CAnimObject &aoOther) {
      ASSERT(FALSE);
    };
    
    inline const CAnimObject &operator=(const CAnimObject &aoOther) {
      ASSERT(FALSE);
      return *this;
    };

    // Clip frame index to be inside valid range (wrap around for looping anims)
    INDEX ClipFrame(INDEX iFrame) const;

    // Loop anims forward
    ENGINE_API void NextAnim(void);

    // Loop anims backward
    ENGINE_API void PrevAnim(void);

    // Loop frames forward
    ENGINE_API void NextFrame(void);

    // Loop frames backward
    ENGINE_API void PrevFrame(void);

    // Select frame in given time offset
    ENGINE_API void SelectFrameInTime(TIME tmOffset);

    // Select first frame
    ENGINE_API void FirstFrame(void);

    // Select last frame
    ENGINE_API void LastFrame(void);

    // Test if some updateable object is up to date with this anim object.
    ENGINE_API BOOL IsUpToDate(const CUpdateable &ud) const;

    // Write and read
    void Write_t(CTStream *ostrFile); // throw char *
    void Read_t(CTStream *istrFile); // throw char *

    // Get animation's info
    ENGINE_API void GetAnimInfo(INDEX iAnimNo, CAnimInfo &aiInfo) const;

    // Attach data to this object
    ENGINE_API void SetData(CAnimData *pAD);

    // Obtain animation and set it for this object
    ENGINE_API void SetData_t(const CTFileName &fnmAnim); // throw char *

    // Get current anim data ptr
    __forceinline CAnimData *GetData() {
      return ao_AnimData;
    };

    // Get animation's length
    ENGINE_API FLOAT GetCurrentAnimLength(void) const;
    ENGINE_API FLOAT GetAnimLength(INDEX iAnim) const;

    // Get number of animations in current anim data
    ENGINE_API INDEX GetAnimsCt() const;

    // Check if animation has finished
    ENGINE_API BOOL IsAnimFinished(void) const;

    // Get passed time from start of animation
    ENGINE_API TIME GetPassedTime(void) const;

    // Start new animation -- obsolete.
    ENGINE_API void StartAnim(INDEX iNew);

    // Start playing an animation.
    ENGINE_API void PlayAnim(INDEX iNew, ULONG ulFlags);

    // Seamlessly continue playing another animation from same point
    ENGINE_API void SwitchToAnim(INDEX iNew);

    // Set new animation but doesn't starts it
    ENGINE_API void SetAnim(INDEX iNew);

    // Reset anim (restart)
    ENGINE_API void ResetAnim();

    // Pause current animation
    ENGINE_API void PauseAnim();

    // Continue paused animation.
    ENGINE_API void ContinueAnim();

    // Offset the animation phase
    ENGINE_API void OffsetPhase(TIME tm);

    // Retrieve paused flag
    ENGINE_API BOOL IsPaused(void);

    // Get the current animation number
    ENGINE_API INDEX GetAnim(void) const;

    // Get the current frame number
    ENGINE_API INDEX GetFrame(void) const;

    // Get number of frames in the current anim
    ENGINE_API INDEX GetFramesInCurrentAnim(void) const;

    // Get information for linear interpolation beetween frames
    ENGINE_API void GetFrame(INDEX &iFrame0, INDEX &iFrame1, FLOAT &fRatio) const;
};

#endif /* include-once check. */
