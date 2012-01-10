/*
 * Copyright (C) 2010-2012 Strawberry-Pr0jcts <http://strawberry-pr0jcts.com/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file
    \ingroup u2w
*/

#include "Opcodes.h"
#include "WorldSession.h"

// Correspondence between opcodes and their names
OpcodeHandler opcodeTable[NUM_MSG_TYPES];
OpcodeTableContainer opcodeTableMap;

uint16 opcodesEnumToNumber[NUM_OPCODES];

void OpcodeTableHandler::LoadOpcodesFromDB()
{
    QueryResult* result = WorldDatabase.Query("SELECT OpcodeName, OpcodeValue FROM Opcodes WHERE ClientBuild > 0");
    if (!result)
        return;

    uint32 count = 0;
    do
    {
        Field *fields = result->Fetch();

        std::string OpcodeName = fields[0].GetString();
        uint16 OpcodeValue     = fields[1].GetUInt16();

        opcodeTableMap[OpcodeName] = OpcodeValue;

        count++;
    }
    while (result->NextRow());

    delete result;

    sLog.outString(">> Loaded %u opcode definitions", count);
    sLog.outString();
}

uint16 OpcodeTableHandler::GetOpcodeTable(const char* name)
{
    OpcodeTableContainer::iterator itr = opcodeTableMap.find(std::string(name));
    if (itr != opcodeTableMap.end())
        return itr->second;

    return NULL;
}

static void DefineOpcode(Opcodes opcodeEnum, const char* name, SessionStatus status, PacketProcessing packetProcessing, void (WorldSession::*handler)(WorldPacket& recvPacket) )
{
    uint16 opcode = sOpcodeTableHandler->GetOpcodeTable(name);

    if (opcode > 0)
    {
        opcodesEnumToNumber[opcodeEnum] = opcode;

        opcodeTable[opcode].name = name;
        opcodeTable[opcode].status = status;
        opcodeTable[opcode].packetProcessing = packetProcessing;
        opcodeTable[opcode].handler = handler;
        opcodeTable[opcode].opcodeEnum = opcodeEnum;
    }
    else
        sLog.outError("SOE: No valid value for %s", name); // Should be removed later. One opcode have the value 0
}

#define OPCODE( name, status, packetProcessing, handler ) DefineOpcode( name, #name, status, packetProcessing, handler )

void InitOpcodeTable()
{
    for( int i = 0; i < NUM_MSG_TYPES; ++i )
    {
        opcodeTable[i].name = "UNKNOWN";
        opcodeTable[i].status = STATUS_NEVER;
        opcodeTable[i].packetProcessing = PROCESS_INPLACE;
        opcodeTable[i].handler = &WorldSession::Handle_NULL;
    }

    // ClientAuthConnection
    OPCODE(MSG_CHECK_CONNECTION,              STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess           );
    OPCODE(SMSG_AUTH_CHALLENGE,               STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(CMSG_AUTH_SESSION,                 STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess           );
    OPCODE(SMSG_AUTH_RESPONSE,                STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Warden
    OPCODE(SMSG_WARDEN_DATA,                  STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Addons
    OPCODE(SMSG_ADDON_INFO,                   STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Cache
    OPCODE(SMSG_CLIENTCACHE_VERSION,          STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Char list
    OPCODE(CMSG_CHAR_ENUM,                    STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleCharEnumOpcode           );
    
    // Realm list
    OPCODE(CMSG_REALM_SPLIT,                  STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleRealmSplitOpcode         );

    // Char list
    OPCODE(SMSG_CHAR_ENUM,                    STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Realm list
    OPCODE(SMSG_REALM_SPLIT,                  STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    
    // Char list
    OPCODE(SMSG_COMPRESSED_CHAR_ENUM,         STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    
    // Char creation
    OPCODE(CMSG_CHAR_CREATE,                  STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleCharCreateOpcode         );
    OPCODE(SMSG_CHAR_CREATE,                  STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Time / Ping
    OPCODE(CMSG_READY_FOR_ACCOUNT_DATA_TIMES, STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleReadyForAccountDataTimesOpcode);
    OPCODE(SMSG_ACCOUNT_DATA_TIMES,           STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_TIME_SYNC_REQ,                STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(CMSG_PING,                         STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess           );
    OPCODE(SMSG_PONG,                         STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // Player
    OPCODE(SMSG_POWER_UPDATE,                 STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // World login
    OPCODE(CMSG_LOADING_SCREEN_NOTIFY,        STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleLoadingScreenNotify      );
    OPCODE(CMSG_PLAYER_LOGIN,                 STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandlePlayerLoginOpcode        );
    OPCODE(SMSG_UPDATE_OBJECT,                STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_COMPRESSED_UPDATE_OBJECT,     STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_NEW_WORLD,                    STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_INIT_WORLD_STATES,            STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_LOGIN_VERIFY_WORLD,           STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_ACTION_BUTTONS,               STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_FEATURE_SYSTEM_STATUS,        STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );
    OPCODE(SMSG_TUTORIAL_FLAGS,               STATUS_NEVER,  PROCESS_INPLACE,      &WorldSession::Handle_ServerSide              );

    // WBD Query handler
    OPCODE(CMSG_NAME_QUERY,                   STATUS_LOGGEDIN, PROCESS_THREADUNSAFE, &WorldSession::HandleNameQueryOpcode           );
    OPCODE(SMSG_NAME_QUERY_RESPONSE,          STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    OPCODE(CMSG_PAGE_TEXT_QUERY,              STATUS_LOGGEDIN, PROCESS_THREADUNSAFE, &WorldSession::HandlePageTextQueryOpcode       );
    OPCODE(SMSG_PAGE_TEXT_QUERY_RESPONSE,     STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    OPCODE(CMSG_QUEST_QUERY,                  STATUS_LOGGEDIN, PROCESS_THREADUNSAFE, &WorldSession::HandleQuestQueryOpcode          );
    OPCODE(SMSG_QUEST_QUERY_RESPONSE,         STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    OPCODE(CMSG_GAMEOBJECT_QUERY,             STATUS_LOGGEDIN, PROCESS_INPLACE,      &WorldSession::HandleGameObjectQueryOpcode     );
    OPCODE(CMSG_CREATURE_QUERY,               STATUS_LOGGEDIN, PROCESS_INPLACE,      &WorldSession::HandleCreatureQueryOpcode       );
    OPCODE(SMSG_CREATURE_QUERY_RESPONSE,      STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    OPCODE(CMSG_NPC_TEXT_QUERY,               STATUS_LOGGEDIN, PROCESS_THREADUNSAFE, &WorldSession::HandleNpcTextQueryOpcode        );
    OPCODE(SMSG_NPC_TEXT_UPDATE,              STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    OPCODE(SMSG_GAMEOBJECT_QUERY_RESPONSE,    STATUS_NEVER,    PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
}
