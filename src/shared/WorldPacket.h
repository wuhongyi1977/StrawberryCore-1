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

#ifndef STRAWBERRYSERVER_WORLDPACKET_H
#define STRAWBERRYSERVER_WORLDPACKET_H

#include "Common.h"
#include "ByteBuffer.h"

// Note: m_opcode and size stored in platfom dependent format
// ignore endianess until send, and converted at receive
class WorldPacket : public ByteBuffer
{
    public:

        // just container for later use
        WorldPacket()                                       : ByteBuffer(0), m_opcode(0)
        {
        }
                                                            // just container for later use
        explicit WorldPacket(Opcodes enumVal, size_t res=200) : ByteBuffer(res)
        {
            m_opcode = LookupOpcodeNumber(enumVal);
        }

        explicit WorldPacket(uint32 opcode, size_t res = 200) : ByteBuffer(res), m_opcode(opcode)
        {
        }

        // copy constructor
        WorldPacket(const WorldPacket &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode)
        {
        }

        void Initialize(Opcodes enumVal, size_t newres=200)
        {
            Initialize(LookupOpcodeNumber(enumVal), newres);
        }

        void Initialize(uint32 opcode, size_t newres = 200)
        {
            clear();
            _storage.reserve(newres);

            m_opcode = opcode;
        }

        uint32 GetOpcode() const { return m_opcode; }
        Opcodes GetOpcodeEnum() const { return LookupOpcodeEnum(m_opcode); }
        void SetOpcode(uint32 opcode) { m_opcode = opcode; }
        void SetOpcode(Opcodes enumVal) { m_opcode = LookupOpcodeNumber(enumVal); }

        void ReadByteMask(uint8& b)
        {
            b = ReadBit() ? 1 : 0;
        }
        void ReadByteSeq(uint8& b)
        {
            if (b != 0)
                b ^= read<uint8>();
        }

        void WriteByteMask(uint8 b)
        {
            WriteBit(b);
        }
        void WriteByteSeq(uint8 b)
        {
            if (b != 0)
                append<uint8>(b ^ 1);
        }

    protected:
        uint16 m_opcode;
};
#endif
