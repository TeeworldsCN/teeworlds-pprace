/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include <game/server/gamemodes/DDRace.h>

#include "portal.h"

CPortal::CPortal(CGameWorld *pGameWorld, vec2 Pos, int Direction, int Owner) :
		CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
  m_Owner = Owner;
  m_Active = false;
	m_Direction = Direction;
	Move(Pos);
	
	m_pPair = 0;

	GameWorld()->InsertEntity(this);
}

int CPortal::Team()
{
  CCharacter *OwnerChar = 0;
	if(m_Owner >= 0)
		OwnerChar = GameServer()->GetPlayerChar(m_Owner);
  if(!OwnerChar)
    return -1;
  return OwnerChar->Team();
}
  
void CPortal::Move(vec2 Pos)
{
	m_Pos = Pos;
	m_Pos.x = int(round(m_Pos.x/32))*32;
	m_Pos.y = int(round(m_Pos.y/32))*32;
	
	bool h = (m_Direction == PORTAL_UP || m_Direction == PORTAL_DOWN);
	m_From.x = m_To.x = (h ? m_Pos.x-16 : m_Pos.x);
	m_From.y = m_To.y = (h ? m_Pos.y : m_Pos.y-16);

  //Move to border
  if(m_Direction == PORTAL_UP)
  {
    m_From.y -= 32;
    m_To.y -= 32;
  }
  
  if(m_Direction == PORTAL_LEFT)
  {
    m_From.x -= 32;
    m_To.x -= 32;
  }
  
	//Open Portal to size 3 tiles
	if(h)
	{
  	m_From.x -= 48;
  	m_To.x += 48;
	}
	else
	{
  	m_From.y -= 48;
  	m_To.y += 48;
	}
}

void CPortal::Reset()
{

}

void CPortal::Tick()
{

}

void CPortal::Snap(int SnappingClient)
{
  if(!m_Active)
    return;
    
  CCharacter *Char = GameServer()->GetPlayerChar(SnappingClient);
  if (Char->Team() != Team())
    return;
    
	if (NetworkClipped(SnappingClient, m_Pos)
			&& NetworkClipped(SnappingClient, m_To))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(
			NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	pObj->m_X = (int) m_From.x;
	pObj->m_Y = (int) m_From.y;
	pObj->m_FromX = (int) m_To.x;
	pObj->m_FromY = (int) m_To.y;
	pObj->m_StartTick = Server()->Tick();
}
