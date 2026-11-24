#include "CPU6502.h"
#include "Bus.h"

CPU6502::CPU6502() {
    using a = CPU6502;
    #define OP(name, addr, cyc) { #name, &a::name, &a::addr, cyc }

    lookup = {
        OP(BRK, IMM, 7), OP(ORA, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 3), OP(ORA, ZP0, 3), OP(ASL, ZP0, 5), OP(XXX, IMP, 5), OP(PHP, IMP, 3), OP(ORA, IMM, 2), OP(ASL, IMP, 2), OP(XXX, IMP, 2), OP(XXX, IMP, 4), OP(ORA, ABS, 4), OP(ASL, ABS, 6), OP(XXX, IMP, 6),
        OP(BPL, REL, 2), OP(ORA, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(ORA, ZPX, 4), OP(ASL, ZPX, 6), OP(XXX, IMP, 6), OP(CLC, IMP, 2), OP(ORA, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(ORA, ABX, 4), OP(ASL, ABX, 7), OP(XXX, IMP, 7),
        OP(JSR, ABS, 6), OP(AND, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(BIT, ZP0, 3), OP(AND, ZP0, 3), OP(ROL, ZP0, 5), OP(XXX, IMP, 5), OP(PLP, IMP, 4), OP(AND, IMM, 2), OP(ROL, IMP, 2), OP(XXX, IMP, 2), OP(BIT, ABS, 4), OP(AND, ABS, 4), OP(ROL, ABS, 6), OP(XXX, IMP, 6),
        OP(BMI, REL, 2), OP(AND, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(AND, ZPX, 4), OP(ROL, ZPX, 6), OP(XXX, IMP, 6), OP(SEC, IMP, 2), OP(AND, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(AND, ABX, 4), OP(ROL, ABX, 7), OP(XXX, IMP, 7),
        OP(RTI, IMP, 6), OP(EOR, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 3), OP(EOR, ZP0, 3), OP(LSR, ZP0, 5), OP(XXX, IMP, 5), OP(PHA, IMP, 3), OP(EOR, IMM, 2), OP(LSR, IMP, 2), OP(XXX, IMP, 2), OP(JMP, ABS, 3), OP(EOR, ABS, 4), OP(LSR, ABS, 6), OP(XXX, IMP, 6),
        OP(BVC, REL, 2), OP(EOR, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(EOR, ZPX, 4), OP(LSR, ZPX, 6), OP(XXX, IMP, 6), OP(CLI, IMP, 2), OP(EOR, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(EOR, ABX, 4), OP(LSR, ABX, 7), OP(XXX, IMP, 7),
        OP(RTS, IMP, 6), OP(ADC, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 3), OP(ADC, ZP0, 3), OP(ROR, ZP0, 5), OP(XXX, IMP, 5), OP(PLA, IMP, 4), OP(ADC, IMM, 2), OP(ROR, IMP, 2), OP(XXX, IMP, 2), OP(JMP, IND, 5), OP(ADC, ABS, 4), OP(ROR, ABS, 6), OP(XXX, IMP, 6),
        OP(BVS, REL, 2), OP(ADC, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(ADC, ZPX, 4), OP(ROR, ZPX, 6), OP(XXX, IMP, 6), OP(SEI, IMP, 2), OP(ADC, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(ADC, ABX, 4), OP(ROR, ABX, 7), OP(XXX, IMP, 7),
        OP(XXX, IMM, 2), OP(STA, IZX, 6), OP(XXX, IMM, 2), OP(XXX, IMP, 6), OP(STY, ZP0, 3), OP(STA, ZP0, 3), OP(STX, ZP0, 3), OP(XXX, IMP, 3), OP(DEY, IMP, 2), OP(XXX, IMM, 2), OP(TXA, IMP, 2), OP(XXX, IMP, 2), OP(STY, ABS, 4), OP(STA, ABS, 4), OP(STX, ABS, 4), OP(XXX, IMP, 4),
        OP(BCC, REL, 2), OP(STA, IZY, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 6), OP(STY, ZPX, 4), OP(STA, ZPX, 4), OP(STX, ZPY, 4), OP(XXX, IMP, 4), OP(TYA, IMP, 2), OP(STA, ABY, 5), OP(TXS, IMP, 2), OP(XXX, IMP, 5), OP(XXX, IMP, 5), OP(STA, ABX, 5), OP(XXX, IMP, 5), OP(XXX, IMP, 5),
        OP(LDY, IMM, 2), OP(LDA, IZX, 6), OP(LDX, IMM, 2), OP(XXX, IMP, 6), OP(LDY, ZP0, 3), OP(LDA, ZP0, 3), OP(LDX, ZP0, 3), OP(XXX, IMP, 3), OP(TAY, IMP, 2), OP(LDA, IMM, 2), OP(TAX, IMP, 2), OP(XXX, IMP, 2), OP(LDY, ABS, 4), OP(LDA, ABS, 4), OP(LDX, ABS, 4), OP(XXX, IMP, 4),
        OP(BCS, REL, 2), OP(LDA, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 5), OP(LDY, ZPX, 4), OP(LDA, ZPX, 4), OP(LDX, ZPY, 4), OP(XXX, IMP, 4), OP(CLV, IMP, 2), OP(LDA, ABY, 4), OP(TSX, IMP, 2), OP(XXX, IMP, 4), OP(LDY, ABX, 4), OP(LDA, ABX, 4), OP(LDX, ABY, 4), OP(XXX, IMP, 4),
        OP(CPY, IMM, 2), OP(CMP, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(CPY, ZP0, 3), OP(CMP, ZP0, 3), OP(DEC, ZP0, 5), OP(XXX, IMP, 5), OP(INY, IMP, 2), OP(CMP, IMM, 2), OP(DEX, IMP, 2), OP(XXX, IMP, 2), OP(CPY, ABS, 4), OP(CMP, ABS, 4), OP(DEC, ABS, 6), OP(XXX, IMP, 6),
        OP(BNE, REL, 2), OP(CMP, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(CMP, ZPX, 4), OP(DEC, ZPX, 6), OP(XXX, IMP, 6), OP(CLD, IMP, 2), OP(CMP, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(CMP, ABX, 4), OP(DEC, ABX, 7), OP(XXX, IMP, 7),
        OP(CPX, IMM, 2), OP(SBC, IZX, 6), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(CPX, ZP0, 3), OP(SBC, ZP0, 3), OP(INC, ZP0, 5), OP(XXX, IMP, 5), OP(INX, IMP, 2), OP(SBC, IMM, 2), OP(NOP, IMP, 2), OP(XXX, IMP, 2), OP(CPX, ABS, 4), OP(SBC, ABS, 4), OP(INC, ABS, 6), OP(XXX, IMP, 6),
        OP(BEQ, REL, 2), OP(SBC, IZY, 5), OP(XXX, IMP, 2), OP(XXX, IMP, 8), OP(XXX, IMP, 4), OP(SBC, ZPX, 4), OP(INC, ZPX, 6), OP(XXX, IMP, 6), OP(SED, IMP, 2), OP(SBC, ABY, 4), OP(XXX, IMP, 2), OP(XXX, IMP, 7), OP(XXX, IMP, 4), OP(SBC, ABX, 4), OP(INC, ABX, 7), OP(XXX, IMP, 7)
    };
    #undef OP
}

CPU6502::~CPU6502() {}

uint8_t CPU6502::read(uint16_t a) { return bus->read(a, false); }
void CPU6502::write(uint16_t a, uint8_t d) { bus->write(a, d); }

uint8_t CPU6502::GetFlag(FLAGS6502 f) { return ((status & f) > 0) ? 1 : 0; }
void CPU6502::SetFlag(FLAGS6502 f, bool v) { if (v) status |= f; else status &= ~f; }

void CPU6502::clock() {
    if (cycles == 0) {
        opcode = read(pc);
        SetFlag(U, true);
        pc++;

        uint8_t cycles_req = lookup[opcode].cycles;
        uint8_t addrcycle1 = (this->*lookup[opcode].addrmode)();
        uint8_t addrcycle2 = (this->*lookup[opcode].operate)();

        cycles_req += (addrcycle1 & addrcycle2);
        cycles = cycles_req;
        SetFlag(U, true);
    }
    cycles--;
}

void CPU6502::reset() {
    addr_abs = 0xFFFC;
    uint16_t lo = read(addr_abs + 0);
    uint16_t hi = read(addr_abs + 1);
    pc = (hi << 8) | lo;
    a = 0; x = 0; y = 0; stp = 0xFD; status = 0x00 | U;
    addr_rel = 0; addr_abs = 0; fetched = 0;
    cycles = 8;
}

void CPU6502::irq() {
    if (GetFlag(I) == 0) {
        write(0x0100 + stp, (pc >> 8) & 0x00FF); stp--;
        write(0x0100 + stp, pc & 0x00FF); stp--;
        SetFlag(B, 0); SetFlag(U, 1); SetFlag(I, 1);
        write(0x0100 + stp, status); stp--;
        addr_abs = 0xFFFE;
        uint16_t lo = read(addr_abs + 0);
        uint16_t hi = read(addr_abs + 1);
        pc = (hi << 8) | lo;
        cycles = 7;
    }
}

void CPU6502::nmi() {
    write(0x0100 + stp, (pc >> 8) & 0x00FF); stp--;
    write(0x0100 + stp, pc & 0x00FF); stp--;
    SetFlag(B, 0); SetFlag(U, 1); SetFlag(I, 1);
    write(0x0100 + stp, status); stp--;
    addr_abs = 0xFFFA;
    uint16_t lo = read(addr_abs + 0);
    uint16_t hi = read(addr_abs + 1);
    pc = (hi << 8) | lo;
    cycles = 8;
}

bool CPU6502::complete() { return cycles == 0; }

uint8_t CPU6502::fetch() {
    if (!(lookup[opcode].addrmode == &CPU6502::IMP)) fetched = read(addr_abs);
    return fetched;
}

// Addressing Modes
uint8_t CPU6502::IMP() { fetched = a; return 0; }
uint8_t CPU6502::IMM() { addr_abs = pc++; return 0; }
uint8_t CPU6502::ZP0() { addr_abs = read(pc); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t CPU6502::ZPX() { addr_abs = (read(pc) + x); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t CPU6502::ZPY() { addr_abs = (read(pc) + y); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t CPU6502::REL() { addr_rel = read(pc); pc++; if (addr_rel & 0x80) addr_rel |= 0xFF00; return 0; }
uint8_t CPU6502::ABS() { uint16_t lo = read(pc); pc++; uint16_t hi = read(pc); pc++; addr_abs = (hi << 8) | lo; return 0; }
uint8_t CPU6502::ABX() { uint16_t lo = read(pc); pc++; uint16_t hi = read(pc); pc++; addr_abs = (hi << 8) | lo; addr_abs += x; return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0; }
uint8_t CPU6502::ABY() { uint16_t lo = read(pc); pc++; uint16_t hi = read(pc); pc++; addr_abs = (hi << 8) | lo; addr_abs += y; return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0; }
uint8_t CPU6502::IND() { uint16_t ptr_lo = read(pc); pc++; uint16_t ptr_hi = read(pc); pc++; uint16_t ptr = (ptr_hi << 8) | ptr_lo; if (ptr_lo == 0x00FF) addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr); else addr_abs = (read(ptr + 1) << 8) | read(ptr); return 0; }
uint8_t CPU6502::IZX() { uint16_t t = read(pc); pc++; uint16_t lo = read((t + (uint16_t)x) & 0x00FF); uint16_t hi = read((t + (uint16_t)x + 1) & 0x00FF); addr_abs = (hi << 8) | lo; return 0; }
uint8_t CPU6502::IZY() { uint16_t t = read(pc); pc++; uint16_t lo = read(t & 0x00FF); uint16_t hi = read((t + 1) & 0x00FF); addr_abs = (hi << 8) | lo; addr_abs += y; return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0; }

// Instructions
uint8_t CPU6502::ADC() { fetch(); uint16_t temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)GetFlag(C); SetFlag(C, temp > 255); SetFlag(Z, (temp & 0x00FF) == 0); SetFlag(N, temp & 0x80); SetFlag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080); a = temp & 0x00FF; return 1; }
uint8_t CPU6502::SBC() { fetch(); uint16_t value = ((uint16_t)fetched) ^ 0x00FF; uint16_t temp = (uint16_t)a + value + (uint16_t)GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(Z, ((temp & 0x00FF) == 0)); SetFlag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080); SetFlag(N, temp & 0x0080); a = temp & 0x00FF; return 1; }

uint8_t CPU6502::AND() { fetch(); a = a & fetched; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 1; }
uint8_t CPU6502::ORA() { fetch(); a = a | fetched; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 1; }
uint8_t CPU6502::EOR() { fetch(); a = a ^ fetched; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 1; }

uint8_t CPU6502::ASL() { fetch(); uint16_t temp = (uint16_t)fetched << 1; SetFlag(C, (temp & 0xFF00) > 0); SetFlag(Z, (temp & 0x00FF) == 0x00); SetFlag(N, temp & 0x80); if (lookup[opcode].addrmode == &CPU6502::IMP) a = temp & 0x00FF; else write(addr_abs, temp & 0x00FF); return 0; }
uint8_t CPU6502::LSR() { fetch(); SetFlag(C, fetched & 0x0001); uint16_t temp = (uint16_t)fetched >> 1; SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); if (lookup[opcode].addrmode == &CPU6502::IMP) a = temp & 0x00FF; else write(addr_abs, temp & 0x00FF); return 0; }
uint8_t CPU6502::ROL() { fetch(); uint16_t temp = (uint16_t)(fetched << 1) | GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); if (lookup[opcode].addrmode == &CPU6502::IMP) a = temp & 0x00FF; else write(addr_abs, temp & 0x00FF); return 0; }
uint8_t CPU6502::ROR() { fetch(); uint16_t temp = (uint16_t)(GetFlag(C) << 7) | (fetched >> 1); SetFlag(C, fetched & 0x01); SetFlag(Z, (temp & 0x00FF) == 0x00); SetFlag(N, temp & 0x0080); if (lookup[opcode].addrmode == &CPU6502::IMP) a = temp & 0x00FF; else write(addr_abs, temp & 0x00FF); return 0; }

uint8_t CPU6502::BCC() { if (GetFlag(C) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BCS() { if (GetFlag(C) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BEQ() { if (GetFlag(Z) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BNE() { if (GetFlag(Z) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BMI() { if (GetFlag(N) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BPL() { if (GetFlag(N) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BVC() { if (GetFlag(V) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t CPU6502::BVS() { if (GetFlag(V) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }

uint8_t CPU6502::BIT() { fetch(); uint16_t temp = a & fetched; SetFlag(Z, (temp & 0x00FF) == 0x00); SetFlag(N, fetched & (1 << 7)); SetFlag(V, fetched & (1 << 6)); return 0; }

uint8_t CPU6502::CLC() { SetFlag(C, false); return 0; }
uint8_t CPU6502::SEC() { SetFlag(C, true); return 0; }
uint8_t CPU6502::CLD() { SetFlag(D, false); return 0; }
uint8_t CPU6502::SED() { SetFlag(D, true); return 0; }
uint8_t CPU6502::CLI() { SetFlag(I, false); return 0; }
uint8_t CPU6502::SEI() { SetFlag(I, true); return 0; }
uint8_t CPU6502::CLV() { SetFlag(V, false); return 0; }

uint8_t CPU6502::CMP() { fetch(); uint16_t temp = (uint16_t)a - (uint16_t)fetched; SetFlag(C, a >= fetched); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); return 1; }
uint8_t CPU6502::CPX() { fetch(); uint16_t temp = (uint16_t)x - (uint16_t)fetched; SetFlag(C, x >= fetched); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); return 0; }
uint8_t CPU6502::CPY() { fetch(); uint16_t temp = (uint16_t)y - (uint16_t)fetched; SetFlag(C, y >= fetched); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); return 0; }

uint8_t CPU6502::DEC() { fetch(); uint16_t temp = fetched - 1; write(addr_abs, temp & 0x00FF); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); return 0; }
uint8_t CPU6502::DEX() { x--; SetFlag(Z, x == 0x00); SetFlag(N, x & 0x80); return 0; }
uint8_t CPU6502::DEY() { y--; SetFlag(Z, y == 0x00); SetFlag(N, y & 0x80); return 0; }
uint8_t CPU6502::INC() { fetch(); uint16_t temp = fetched + 1; write(addr_abs, temp & 0x00FF); SetFlag(Z, (temp & 0x00FF) == 0x0000); SetFlag(N, temp & 0x0080); return 0; }
uint8_t CPU6502::INX() { x++; SetFlag(Z, x == 0x00); SetFlag(N, x & 0x80); return 0; }
uint8_t CPU6502::INY() { y++; SetFlag(Z, y == 0x00); SetFlag(N, y & 0x80); return 0; }

uint8_t CPU6502::JMP() { pc = addr_abs; return 0; }
uint8_t CPU6502::JSR() { pc--; write(0x0100 + stp, (pc >> 8) & 0x00FF); stp--; write(0x0100 + stp, pc & 0x00FF); stp--; pc = addr_abs; return 0; }
uint8_t CPU6502::RTS() { uint16_t lo = read(0x0100 + ++stp); uint16_t hi = read(0x0100 + ++stp); pc = (hi << 8) | lo; pc++; return 0; }

uint8_t CPU6502::LDA() { fetch(); a = fetched; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 1; }
uint8_t CPU6502::LDX() { fetch(); x = fetched; SetFlag(Z, x == 0x00); SetFlag(N, x & 0x80); return 1; }
uint8_t CPU6502::LDY() { fetch(); y = fetched; SetFlag(Z, y == 0x00); SetFlag(N, y & 0x80); return 1; }

uint8_t CPU6502::STA() { write(addr_abs, a); return 0; }
uint8_t CPU6502::STX() { write(addr_abs, x); return 0; }
uint8_t CPU6502::STY() { write(addr_abs, y); return 0; }

uint8_t CPU6502::TAX() { x = a; SetFlag(Z, x == 0x00); SetFlag(N, x & 0x80); return 0; }
uint8_t CPU6502::TAY() { y = a; SetFlag(Z, y == 0x00); SetFlag(N, y & 0x80); return 0; }
uint8_t CPU6502::TSX() { x = stp; SetFlag(Z, x == 0x00); SetFlag(N, x & 0x80); return 0; }
uint8_t CPU6502::TXA() { a = x; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 0; }
uint8_t CPU6502::TXS() { stp = x; return 0; }
uint8_t CPU6502::TYA() { a = y; SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 0; }

uint8_t CPU6502::PHA() { write(0x0100 + stp, a); stp--; return 0; }
uint8_t CPU6502::PHP() { write(0x0100 + stp, status | B | U); SetFlag(B, 0); SetFlag(U, 0); stp--; return 0; }
uint8_t CPU6502::PLA() { stp++; a = read(0x0100 + stp); SetFlag(Z, a == 0x00); SetFlag(N, a & 0x80); return 0; }
uint8_t CPU6502::PLP() { stp++; status = read(0x0100 + stp); SetFlag(U, 1); return 0; }

uint8_t CPU6502::BRK() { pc++; SetFlag(I, 1); write(0x0100 + stp, (pc >> 8) & 0x00FF); stp--; write(0x0100 + stp, pc & 0x00FF); stp--; SetFlag(B, 1); write(0x0100 + stp, status); stp--; SetFlag(B, 0); pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8); return 0; }
uint8_t CPU6502::RTI() { stp++; status = read(0x0100 + stp); status &= ~B; status &= ~U; stp++; uint16_t lo = read(0x0100 + stp); stp++; uint16_t hi = read(0x0100 + stp); pc = (hi << 8) | lo; return 0; }
uint8_t CPU6502::NOP() { switch (opcode) { case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC: return 1; break; } return 0; }
uint8_t CPU6502::XXX() { return 0; }
