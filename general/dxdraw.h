
typedef int intx100; /** 100倍していることを示すint型 */

/** dxdrawで使う基準点の列挙型 */
typedef enum dxdraw_anchor_e {
	DXDRAW_ANCHOR_TOP_LEFT = 0,  /** 上の左 */
	DXDRAW_ANCHOR_TOP_CENTRE,    /** 上の真ん中 */
	DXDRAW_ANCHOR_TOP_RIGHT,     /** 上の右 */
	DXDRAW_ANCHOR_CENTRE_LEFT,   /** 真ん中の左 */
	DXDRAW_ANCHOR_CENTRE,        /** 真ん中 */
	DXDRAW_ANCHOR_CENTRE_RIGHT,  /** 真ん中の右 */
	DXDRAW_ANCHOR_BOTTOM_LEFT,   /** 下の左 */
	DXDRAW_ANCHOR_BOTTOM_CENTRE, /** 下の真ん中 */
	DXDRAW_ANCHOR_BOTTOM_RIGHT,  /** 下の右 */
} dxdraw_anchor_t;

extern void DrawGraphAnchor(int x, int y, int pic, dxdraw_anchor_t anchor);
extern void DrawZoomGraphAnchor(int x, int y, intx100 size, int pic, dxdraw_anchor_t anchor);
extern void DrawDeformationPic(int x, int y, double sizeX, double sizeY, int rot, int handle);
extern void _DrawDeformationPic4(int x, int y, double sizeX, double sizeY, int handle);
extern void _DrawDeformationPic3(int x, int y, double size, int rot, int handle);
extern void _DrawDeformationPic2(int x, int y, double sizeX, double sizeY, int rot, int handle);
