#ifndef _STATE_TRACKER_H_
#define _STATE_TRACKER_H_


template <class T>
class StateTracker
{
	public:

		StateTracker( T normalState, T activeState, double activeDurationS )
			: 	mNormalState( normalState ),
				mActiveState( activeState ),
				mActiveDurationS( activeDurationS )
		{
		}

		void Activate()
		{
			mElapsedS = 0.0;
			mIsActive = true;
		}

		void Update( double elapsedS )
		{
			if( !mIsActive )
				return;

			mElapsedS += elapsedS;
			if( mElapsedS < mActiveDurationS )
				return;

			mElapsedS = 0.0;
			mIsActive = false;											
		}

		bool IsActive()
		{
			return mIsActive;
		}

		T State()
		{
			if( mIsActive )
				return mActiveState;

			return mNormalState;
		}


	private:


		T 				mNormalState;
		T 				mActiveState;
		bool 			mIsActive = false;
		double 			mElapsedS = 0.0;
		const double 	mActiveDurationS;


}; // StateTracker


#endif // _STATE_TRACKER_H_
