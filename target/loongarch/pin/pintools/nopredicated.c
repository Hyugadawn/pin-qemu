#include "pintool.h"
#include "../../instrument/decoder/disasm.h"
#include "../../instrument/decoder/la_print.h"
#include "../ins_inspection.h"


/* INS */
static UINT64 icount = 0;

static VOID show_ins(UINT64 pc, UINT32 opcode)
{
    ++icount;
    /* print ins */
    char msg[128];
    Ins ins;
    la_disasm(opcode, &ins);
    sprint_ins(&ins, msg);
    fprintf(stderr, "[thread %d] %lu\tpc: 0x%lx\t%s\n", PIN_ThreadId(), icount, pc, msg);
}

static VOID Instruction(INS ins, VOID* v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)show_ins, IARG_UINT64, ins->pc, IARG_UINT64, ins->opcode, IARG_END);
}

static VOID Init(VOID)
{
}

static VOID Fini(INT32 code, VOID* v)
{
    fprintf(stderr, "Total Ins Count: %lu\n", icount);
    /* fprintf(stderr, "BBL: %ld, INS: %ld, Avg: %f INSs/BBL\n", bbl_exec_nr, ins_exec_nr, (double)ins_exec_nr / bbl_exec_nr); */
}
 
static INT32 Usage(void)
{
    return -1;
}
 
int main(int argc, char* argv[])
{
    if (PIN_Init(argc, argv)) return Usage();
 
    Init();

    INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddFiniFunction(Fini, 0);
 
    return 0;
}
