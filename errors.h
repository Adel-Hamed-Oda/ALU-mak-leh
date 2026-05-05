typedef enum EXCEPTION {
    SUCCESS = 0,
    GENERAL_FAILURE = -1,
    INSTRUCTION_TYPE_J = -2,
    OUT_OF_BOUNDS = -2,
    NAME_NOT_FOUND = -3,
    INDEX_NOT_FOUND = -4,
    ASSEMBLY_ERROR = -5,
    REGISTER_ERROR = -6,
} EXCEPTION;

#define FAIL(x) ((x) < 0)