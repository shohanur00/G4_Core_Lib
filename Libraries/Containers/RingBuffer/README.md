<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<title>Ring Buffer — G4_Core_Lib</title>
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;500;600;700&family=IBM+Plex+Mono:wght@400;500;600&display=swap" rel="stylesheet">
<style>
:root{
  --bg:#11151a; --surface:#171d24; --surface2:#1c232b; --border:#2a333c;
  --text:#dfe5ec; --muted:#8794a3; --accent:#ffb000; --accent-soft:#3a2c10;
  --mono: 'IBM Plex Mono', ui-monospace, monospace;
  --sans: 'Space Grotesk', system-ui, sans-serif;
  box-sizing:border-box;
  padding-top:env(safe-area-inset-top,0px);
  padding-bottom:env(safe-area-inset-bottom,0px);
}
:root:not([data-theme="light"]){ }
:root[data-theme="light"]{
  --bg:#f6f4ef; --surface:#ffffff; --surface2:#f0ede5; --border:#ddd6c8;
  --text:#1c1f24; --muted:#6b6458; --accent:#b5760a; --accent-soft:#f2e2bd;
}
@media (prefers-color-scheme: light){
  :root:not([data-theme="dark"]){
    --bg:#f6f4ef; --surface:#ffffff; --surface2:#f0ede5; --border:#ddd6c8;
    --text:#1c1f24; --muted:#6b6458; --accent:#b5760a; --accent-soft:#f2e2bd;
  }
}
html{scroll-padding-top:env(safe-area-inset-top,0px); scroll-behavior:smooth;}
*{margin:0;padding:0;}
body{
  background:var(--bg); color:var(--text); font-family:var(--sans);
  line-height:1.6; -webkit-font-smoothing:antialiased;
}
.wrap{max-width:760px; margin:0 auto; padding:64px 24px 96px;}
h1,h2,h3{font-family:var(--sans); font-weight:600; letter-spacing:-0.01em;}
code, pre, .mono{font-family:var(--mono);}

/* Hero */
.hero{padding-bottom:40px; border-bottom:1px solid var(--border); margin-bottom:48px;}
.hero-kicker{font-size:14px; color:var(--muted); margin-bottom:10px;}
.hero h1{font-size:40px; line-height:1.1; margin-bottom:14px;}
.hero p{color:var(--muted); font-size:16px; max-width:52ch; margin-bottom:32px;}

/* Buffer diagram */
.diagram{background:var(--surface); border:1px solid var(--border); border-radius:6px; padding:24px; overflow-x:auto;}
.cells{display:flex; gap:2px;}
.cell{width:44px; height:44px; display:flex; align-items:center; justify-content:center;
  background:var(--surface2); border:1px solid var(--border); font-family:var(--mono); font-size:13px; color:var(--muted); flex-shrink:0; position:relative;}
.cell.read{border-color:var(--accent); color:var(--accent);}
.cell.write{border-color:var(--accent); color:var(--accent);}
.ptr{font-family:var(--mono); font-size:11px; color:var(--accent); text-align:center; margin-top:6px;}
.ptr-row{display:flex; gap:2px; margin-top:2px;}
.ptr-cell{width:44px; flex-shrink:0; text-align:center; font-size:11px; font-family:var(--mono); color:var(--muted);}
.ptr-cell.active{color:var(--accent);}

/* sections */
section{margin-bottom:56px;}
.section-head{display:flex; align-items:baseline; gap:12px; margin-bottom:18px; border-left:3px solid var(--accent); padding-left:14px;}
.section-head h2{font-size:22px;}
.section-head .idx{font-family:var(--mono); color:var(--muted); font-size:13px;}
p{margin-bottom:14px; max-width:68ch;}
.muted{color:var(--muted);}

/* feature grid */
.feat-grid{display:grid; grid-template-columns:1fr 1fr; gap:10px 20px;}
.feat-grid div{font-size:14.5px; padding:8px 0; border-bottom:1px solid var(--border); color:var(--text);}
.feat-grid div::before{content:"·"; color:var(--accent); margin-right:8px; font-weight:700;}

/* code */
pre{background:#0c0f13; border:1px solid var(--border); border-radius:6px; padding:16px 18px; overflow-x:auto; font-size:13.5px; line-height:1.55; margin:14px 0;}
:root[data-theme="light"] pre, :root:not([data-theme="dark"]) pre{}
@media (prefers-color-scheme: light){ pre{background:#efece3;} }
:root[data-theme="light"] pre{background:#efece3;}
code.inline{background:var(--surface2); border:1px solid var(--border); padding:1px 6px; border-radius:4px; font-size:0.88em;}

/* api cards */
.api-group{margin-bottom:30px;}
.api-group h3{font-size:13px; text-transform:none; color:var(--muted); font-weight:600; margin-bottom:12px; font-family:var(--mono);}
.api-card{background:var(--surface); border:1px solid var(--border); border-radius:6px; padding:16px 18px; margin-bottom:10px;}
.api-card .sig{font-family:var(--mono); font-size:13.5px; color:var(--accent); margin-bottom:8px; word-break:break-word;}
.api-card p{font-size:14.5px; color:var(--muted); margin:0; max-width:none;}

/* flow diagram */
.flow{display:flex; align-items:center; gap:10px; flex-wrap:wrap; font-family:var(--mono); font-size:13px;}
.flow .box{background:var(--surface); border:1px solid var(--border); border-radius:6px; padding:10px 14px;}
.flow .arrow{color:var(--accent);}

table{width:100%; border-collapse:collapse; font-size:14px; margin:14px 0;}
th,td{text-align:left; padding:8px 10px; border-bottom:1px solid var(--border);}
th{color:var(--muted); font-weight:500; font-size:12.5px;}

footer{border-top:1px solid var(--border); padding-top:24px; color:var(--muted); font-size:13px;}
a{color:var(--accent);}
</style>
</head>
<body>
<div class="wrap">

  <div class="hero">
    <div class="hero-kicker">Libraries / containers / ringbuffer</div>
    <h1>Ring Buffer</h1>
    <p>A static-memory, power-of-two circular buffer for embedded C. No <code class="inline">malloc</code>, no hardware dependency, single-producer / single-consumer by design.</p>

    <div class="diagram">
      <div class="cells">
        <div class="cell">0</div><div class="cell">1</div><div class="cell">2</div>
        <div class="cell">3</div><div class="cell">4</div><div class="cell">5</div>
        <div class="cell read">6</div><div class="cell">7</div>
      </div>
      <div class="ptr-row">
        <div class="ptr-cell"></div><div class="ptr-cell"></div><div class="ptr-cell"></div>
        <div class="ptr-cell"></div><div class="ptr-cell"></div><div class="ptr-cell"></div>
        <div class="ptr-cell active">readIndex</div><div class="ptr-cell"></div>
      </div>
    </div>
  </div>

  <section>
    <div class="section-head"><h2>Features</h2></div>
    <div class="feat-grid">
      <div>Static allocation, no heap</div>
      <div>Power-of-two sizing, mask-based wrap</div>
      <div>Single-byte and bulk read / write</div>
      <div>Peek without removing data</div>
      <div>Discard without copying</div>
      <div>Zero-copy access via PeekBuffer</div>
      <div>Byte search (Find)</div>
      <div>Hardware-independent, unit-testable on PC</div>
    </div>
  </section>

  <section>
    <div class="section-head"><h2>Design</h2></div>
    <p>Two indices track the buffer: <code class="inline">readIndex</code> points to the oldest unread byte, <code class="inline">writeIndex</code> to the next free slot. Both wrap using a bitwise mask rather than modulo:</p>
    <pre>mask  = size - 1;              // size = 8  →  mask = 0b0111
index = (index + 1U) & mask;   // replaces index % size</pre>
    <p>The buffer reserves one slot as a convention: <code class="inline">readIndex == writeIndex</code> unambiguously means <em>empty</em>, so a physical buffer of <code class="inline">N</code> bytes exposes a usable capacity of <code class="inline">N − 1</code>.</p>
  </section>

  <section>
    <div class="section-head"><h2>Status API</h2></div>
    <table>
      <tr><th>Function</th><th>Returns</th></tr>
      <tr><td class="mono">RingBuffer_Empty(rb)</td><td>true when no data is available</td></tr>
      <tr><td class="mono">RingBuffer_Full(rb)</td><td>true when no byte can be written</td></tr>
      <tr><td class="mono">RingBuffer_Capacity(rb)</td><td>usable capacity (N − 1)</td></tr>
      <tr><td class="mono">RingBuffer_Count(rb)</td><td>unread bytes currently stored</td></tr>
      <tr><td class="mono">RingBuffer_Free(rb)</td><td>capacity − count</td></tr>
    </table>
  </section>

  <section>
    <div class="section-head"><h2>Core API</h2></div>

    <div class="api-group">
      <h3>SETUP</h3>
      <div class="api-card">
        <div class="sig">void RingBuffer_Setup(RingBuffer_t *rb, uint8_t *buffer, uint32_t size);</div>
        <p>Initializes a buffer using caller-provided storage. <code class="inline">size</code> must be a power of two, minimum 2.</p>
      </div>
      <div class="api-card">
        <div class="sig">void RingBuffer_Reset(RingBuffer_t *rb);</div>
        <p>Resets read/write positions. Buffer contents are left in place, only the indices move.</p>
      </div>
    </div>

    <div class="api-group">
      <h3>SINGLE-BYTE</h3>
      <div class="api-card">
        <div class="sig">bool RingBuffer_Write(RingBuffer_t *rb, uint8_t byte);</div>
        <p>Writes one byte. Returns false if the buffer is full.</p>
      </div>
      <div class="api-card">
        <div class="sig">bool RingBuffer_Read(RingBuffer_t *rb, uint8_t *byte);</div>
        <p>Reads and removes the oldest byte.</p>
      </div>
      <div class="api-card">
        <div class="sig">bool RingBuffer_Peek(const RingBuffer_t *rb, uint8_t *byte);</div>
        <p>Reads the oldest byte without advancing <code class="inline">readIndex</code>.</p>
      </div>
      <div class="api-card">
        <div class="sig">bool RingBuffer_Discard(RingBuffer_t *rb);</div>
        <p>Advances past the oldest byte without copying it out.</p>
      </div>
    </div>

    <div class="api-group">
      <h3>BULK</h3>
      <div class="api-card">
        <div class="sig">uint32_t RingBuffer_WriteBuffer(rb, const uint8_t *data, uint32_t length);</div>
        <p>Writes up to <code class="inline">length</code> bytes; returns the number actually written.</p>
      </div>
      <div class="api-card">
        <div class="sig">uint32_t RingBuffer_ReadBuffer(rb, uint8_t *data, uint32_t length);</div>
        <p>Reads up to <code class="inline">length</code> bytes; returns the number actually read.</p>
      </div>
      <div class="api-card">
        <div class="sig">uint32_t RingBuffer_DiscardBuffer(rb, uint32_t length);</div>
        <p>Removes multiple bytes without copying them out.</p>
      </div>
    </div>

    <div class="api-group">
      <h3>ZERO-COPY &amp; SEARCH</h3>
      <div class="api-card">
        <div class="sig">const uint8_t *RingBuffer_PeekBuffer(rb, uint32_t *length);</div>
        <p>Returns a direct pointer to the first contiguous readable region. When data wraps, call again after discarding to reach the second region.</p>
      </div>
      <div class="api-card">
        <div class="sig">bool RingBuffer_Find(rb, uint8_t value, uint32_t *offset);</div>
        <p>Searches unread data for a byte; offset is relative to <code class="inline">readIndex</code>. Non-destructive.</p>
      </div>
    </div>
  </section>

  <section>
    <div class="section-head"><h2>Example</h2></div>
    <pre>uint8_t buffer[8];
uint8_t byte;
RingBuffer_t rb;

RingBuffer_Setup(&amp;rb, buffer, sizeof(buffer));

RingBuffer_Write(&amp;rb, 'A');
RingBuffer_Write(&amp;rb, 'B');
RingBuffer_Write(&amp;rb, 'C');

if (RingBuffer_Read(&amp;rb, &amp;byte)) {
    /* byte == 'A' */
}</pre>
  </section>

  <section>
    <div class="section-head"><h2>Typical use</h2></div>
    <div class="flow">
      <div class="box">UART ISR / DMA</div><div class="arrow">→</div>
      <div class="box">Ring Buffer</div><div class="arrow">→</div>
      <div class="box">Protocol parser / app</div>
    </div>
    <p style="margin-top:16px" class="muted">UART RX/TX, CAN message queues, USB streaming, DMA staging, logger backends, and command-line interfaces all follow this producer/consumer shape.</p>
  </section>

  <section>
    <div class="section-head"><h2>Notes &amp; limits</h2></div>
    <div class="feat-grid" style="grid-template-columns:1fr;">
      <div>No STM32 / HAL / CMSIS / RTOS dependency — portable across MCUs</div>
      <div>Not synchronized: safe for single-producer / single-consumer only</div>
      <div>Requires a power-of-two size; use modulo wrapping for arbitrary sizes</div>
    </div>
  </section>

  <footer>Part of <span class="mono">G4_Core_Lib</span>.</footer>
</div>
</body>
</html>
