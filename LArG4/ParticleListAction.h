////////////////////////////////////////////////////////////////////////
/// \file  ParticleListAction.h
/// \brief Use Geant4's user "hooks" to maintain a list of particles generated by Geant4.
///
/// \version $Id: ParticleListAction.h,v 1.3 2010/04/29 15:39:33 seligman Exp $
/// \author  seligman@nevis.columbia.edu
////////////////////////////////////////////////////////////////////////

/// This class implements the LArG4::UserAction interface in order to
/// accumulate a list of particles modeled by Geant4.
//
/// It uses multiple inheritance: it inherits from LArG4::UserAction,
/// in order to take advantage of Geant4's user hooks; it also
/// inherits from cfg::Observer, because it accesses a parameter from
/// an XML configuration file.

#ifndef LArG4_ParticleListAction_h
#define LArG4_ParticleListAction_h

#include "LArG4/ParticleFilters.h" // larg4::PositionInVolumeFilter

#include "SimulationBase/MCParticle.h"
#include "G4Base/UserAction.h"

#include "Geant4/globals.hh"
#include <map>

// Forward declarations.
class G4Event;
class G4Track;
class G4Step;

namespace sim { 
  class ParticleList; 
}

namespace larg4 {

  class ParticleListAction : public g4b::UserAction
  {
  public:
    // Standard constructors and destructors;
    ParticleListAction(double energyCut, bool storeTrajectories=false, bool keepEMShowerDaughters=false);
    virtual ~ParticleListAction();

    // UserActions method that we'll override, to obtain access to
    // Geant4's particle tracks and trajectories.
    virtual void             BeginOfEventAction(const G4Event*);
    virtual void     	     EndOfEventAction  (const G4Event*);
    virtual void     	     PreTrackingAction (const G4Track*);
    virtual void     	     PostTrackingAction(const G4Track*);
    virtual void     	     SteppingAction    (const G4Step* );

    /// Grabs a particle filter
    void ParticleFilter(std::unique_ptr<PositionInVolumeFilter>&& filter)
      { fFilter = std::move(filter); }
  

    // TrackID of the current particle, EveID if the particle is from an EM shower
    static int               GetCurrentTrackID() { return fCurrentTrackID; }
			                                                     
    void                     ResetTrackIDOffset() { fTrackIDOffset = 0;     }

    // Returns the ParticleList accumulated during the current event.
    const sim::ParticleList* GetList() const;

  private:

    // this method will loop over the fParentIDMap to get the 
    // parentage of the provided trackid
    int               	     GetParentage(int trackid) const;

    G4double                 fenergyCut;             ///< The minimum energy for a particle to 		     	  
                                                     ///< be included in the list.
    simb::MCParticle*        fparticle;              ///< The particle and trajectory information 
                                                     ///< for a single particle.		
    sim::ParticleList*       fparticleList;          ///< The accumulated particle information for 
                                                     ///< all particles in the event.	
    G4bool                   fstoreTrajectories;     ///< Whether to store particle trajectories with each particle. 
    std::map<int, int>       fParentIDMap;           ///< key is current track ID, value is parent ID		  
    static int               fCurrentTrackID;        ///< track ID of the current particle, set to eve ID 
                                                     ///< for EM shower particles		
    static int               fTrackIDOffset;         ///< offset added to track ids when running over		  
                                                     ///< multiple MCTruth objects.				  
    bool                     fKeepEMShowerDaughters; ///< whether to keep EM shower secondaries, tertiaries, etc     
    
    std::unique_ptr<PositionInVolumeFilter> fFilter; ///< filter for particles to be kept
    
  };

} // namespace LArG4

#endif // LArG4_ParticleListAction_h
