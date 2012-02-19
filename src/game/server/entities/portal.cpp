/* Teeworlds.cz based on code by
 * (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include <game/server/gamemodes/DDRace.h>

#include "portal.h"

CPortal::CPortal(CGameWorld *pGameWorld, vec2 Pos, int Direction, int Owner, bool IsSecond) :
		CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
  m_ID2 = Server()->SnapNewID();
  
  m_UseS2 = IsSecond;
  m_IDS1 = Server()->SnapNewID();
  if(IsSecond)
    m_IDS2 = Server()->SnapNewID();
  
  m_Owner = Owner;
  m_Active = false;
	m_Direction = Direction;
	Move(Pos);
	
	m_pPair = 0;

  m_SignType = -1;

	GameWorld()->InsertEntity(this);
}

CPortal::~CPortal()
{
  Server()->SnapFreeID(m_ID2);
  Server()->SnapFreeID(m_IDS1);
  if(m_UseS2)
    Server()->SnapFreeID(m_IDS2);
}

bool CPortal::IsIn(vec2 Pos)
{
  //34 - based on PhysSize of Tee (48-14)
  if(m_Direction == PORTAL_UP || m_Direction == PORTAL_DOWN)
  {
    if(absolute(Pos.x-m_Pos.x) >= 34)
      return false;
   
    if((m_Direction == PORTAL_UP) == ((Pos.y-m_Pos.y) > 0))
      return false;
      
    return absolute(Pos.y-m_Pos.y) < 48; //1 tile
  }
  else
  {
    if(absolute(Pos.y-m_Pos.y) >= 34)
      return false;
      
    if((m_Direction == PORTAL_LEFT) == ((Pos.x-m_Pos.x) > 0))
      return false;
      
    return absolute(Pos.x-m_Pos.x) < 48; //1 tile
  }
}

bool CPortal::IsTooClose(vec2 Pos)
{
  float l = m_Pos.x-16;
  float u = m_Pos.y-16;
  float r = m_Pos.x+16;
  float d = m_Pos.y+16;
  
  if(m_Direction == PORTAL_UP || m_Direction == PORTAL_DOWN)
  {
    l -= 96;
    r += 96;
    
    if(m_Direction == PORTAL_UP)
      u -= 96;
    else
      d += 96;
  }
  else
  {
    u -= 96;
    d += 96;
    
    if(m_Direction == PORTAL_LEFT)
      l -= 96;
    else
      r += 96;
  }
  
  return (Pos.x > l && Pos.y > u && Pos.x < r && Pos.y < d);
}

bool CPortal::IsHorizontal()
{
  return (m_Direction == PORTAL_UP || m_Direction == PORTAL_DOWN);
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
  m_Pos.x -= 16;
  m_Pos.y -= 16;
  
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
	
	//Move Signs
  m_S1Pos = m_Pos;
  m_S2Pos = m_Pos;
	if(m_UseS2)
	{
    if(h)
    {
      m_S1Pos.x -= 16;
      m_S2Pos.x += 16;
    }
    else
    {
      m_S1Pos.y -= 16;
      m_S2Pos.y += 16;
    }
	}
	
	switch(m_Direction)
	{
	  case PORTAL_LEFT:
	    m_S1Pos.x -= 16;
      m_S2Pos.x -= 16;
      break;
      
	  case PORTAL_UP:
	    m_S1Pos.y -= 16;
      m_S2Pos.y -= 16;
      break;
      
	  case PORTAL_RIGHT:
	    m_S1Pos.x += 16;
      m_S2Pos.x += 16;
      break;
      
	  case PORTAL_DOWN:
	    m_S1Pos.y += 16;
      m_S2Pos.y += 16;
      break;
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
  
  //Only spectators can see all portals
  if(Char && !GameServer()->m_apPlayers[SnappingClient]->m_ShowOthers) 
  {
    if (Char->Team() != Team())
      return;
  }
   
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
	
	pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(
			NETOBJTYPE_LASER, m_ID2, sizeof(CNetObj_Laser)));
	pObj->m_X = pObj->m_FromX = (int) m_To.x;
	pObj->m_Y = pObj->m_FromY = (int) m_To.y;
	pObj->m_StartTick = Server()->Tick();
	
	//Signs
	if(m_SignType == -1)
	  return;
	  
	CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDS1, sizeof(CNetObj_Pickup)));
	if(!pP)
		return;

	pP->m_X = (int)m_S1Pos.x;
	pP->m_Y = (int)m_S1Pos.y;
	pP->m_Type = m_SignType;
	pP->m_Subtype = 0;
	
	if(m_UseS2)
	{
	  pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDS2, sizeof(CNetObj_Pickup)));
  	if(!pP)
  		return;
  
  	pP->m_X = (int)m_S2Pos.x;
  	pP->m_Y = (int)m_S2Pos.y;
  	pP->m_Type = m_SignType;
  	pP->m_Subtype = 0;
	}
}
