// Sample program for SpinCore PulseBlaster Interpreter.
// 3 short flashes, 3 long flashes, 3 short flashes in a continuous loop.

// 3 Short			
start: 0xFFFFFF, 100ms // All bits on
       0x000000, 100ms // All bits off
       0xFFFFFF, 100ms
       0x000000, 100ms
       0xFFFFFF, 100ms
       0x000000, 100ms
			
// 3 Long
       0xFFFFFF, 300ms
       0x000000, 100ms
       0xFFFFFF, 300ms
       0x000000, 100ms
       0xFFFFFF, 300ms
       0x000000, 100ms
			
// 3 Short			
       0xFFFFFF, 100ms
       0x000000, 100ms
       0xFFFFFF, 100ms
       0x000000, 100ms
       0xFFFFFF, 100ms
       0x000000, 100ms
			
// A pause
       0x000000, 500ms, branch, start // branch to start