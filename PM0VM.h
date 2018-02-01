// im going to need some kind of stack structure
typedef struct activationFrame{
  struct activationFrame *next;
  struct activationFram *prev;
}activationFrame;

typedef struct instruction{
  int po; // opcode
  int r; // reg
  int l; // arguement L
  int m; // arguement M
}instruction;
