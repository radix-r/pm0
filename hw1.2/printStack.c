void printStack(int sp, int bp, int* stack, int lex){
     int i;
     if (bp == 1) {
     	if (lex > 0) {
	   printf("|");
	   }
     }	   
     else {
     	  //Print the lesser lexical level
     	  printStack(bp-1, stack[bp + 2], stack, lex-1);
	  printf("|");
     }
     //Print the stack contents - at the current level
     for (i = bp; i <= sp; i++) {
     	 printf("%3d ", stack[i]);	
     }
}
