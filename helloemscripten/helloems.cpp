#include <emscripten/val.h>
#include <emscripten.h>

using emscripten::val;

// Use thread_local when you want to retrieve & cache a global JS variable once per thread.
thread_local const val document = val::global("document");

// …
//val canvas = val::global("document").call<val>("createElement", val("canvas"));
int main() {
  val canvas = document.call<val>("getElementById", val("canvas"));
  val ctx = canvas.call<val>("getContext", val("2d"));
  ctx.set("fillStyle", "green");
  int startX = 0;
  int startY = 0;
  while(true) {
	  startX += 1;
	  startY += 2;
	  ctx.call<void>("fillRect", startX, startY, 150, 100);
	  emscripten_sleep(10);
  }
}
