// Types 
// --------------------------------------------------------

// bit[] type
var bit[32] n     = 24;
var bit[n]  data0 = 32;

// MP4 File
struct mp4file {
   list{payload};
};

struct payload {
   bit[32] name;
   bit[32] size;
   bit[32] version if name in fullatomlist;

   list{payload} if name in decomposableatoms || 
   bit[(size-sizeof(name)-sizeof(size)-sizeof(version))*8] data; 
};

var list{bit[32]} fullatomname = {00000000, 11111111};

