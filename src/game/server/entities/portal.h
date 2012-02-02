/* Teeworlds.cz based on code by
 * (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_ENTITY_PORTAL_H
#define GAME_SERVER_ENTITY_PORTAL_H

#include <game/server/entity.h>

enum
{
  PORTAL_LEFT = 0,
  PORTAL_UP,
  PORTAL_RIGHT,
  PORTAL_DOWN
};

class CPortal: public CEntity
{
protected:
  int m_ID2;
public:
	int m_Owner;
  CPortal *m_pPair;
  
  vec2 m_From;
  vec2 m_To;
  
	bool m_Active;
	int m_Direction;
  
	CPortal(CGameWorld *pGameWorld, vec2 Pos, int Direction, int Owner);
	~CPortal();
  
  bool IsIn(vec2 Pos); //Entered to portal
  bool IsTooClose(vec2 Pos); //Is new portal position too close?
  bool IsHorizontal(); //Is horizintally placed portal? (PORTAL_UP or PORTAL_DOWN) 
  int Team(); //Portal is in Team
  void Move(vec2 Pos); //Set portal position (Direction needed!)
	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
};

#endif
