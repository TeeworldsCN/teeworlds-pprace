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
public:
	int m_Owner;
  CPortal *m_pPair;
  
  vec2 m_From;
  vec2 m_To;
  
	bool m_Active;
	int m_Direction;
  
	CPortal(CGameWorld *pGameWorld, vec2 Pos, int Direction, int Owner);
  
  bool IsIn(vec2 Pos);
  bool IsHorizontal();
  int Team();
  void Move(vec2 Pos);
	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
};

#endif
