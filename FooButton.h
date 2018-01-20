//  FooButton.h
//  A versatile bitmap button
//  Copyright (c) 1997-2004 Ravi Bhavnani (ravib@ravib.com)
//
//  You are allowed to include the source code in any product (commercial, shareware,
//  freeware or otherwise) when your product is released in binary form.  You are allowed
//  to modify the source code in any way you want except you cannot modify the copyright
//  details at the top of each module.  If you want to distribute source code with your
//  application, then you are only allowed to distribute versions released by the author.
//  This is to maintain a single distribution point for the source code. 

#ifndef FooButton_h
#define FooButton_h

/**
 *  A versatile bitmap button.
 */

class FooButton : public CButton
{
/////////////////////////////////////////////////////////////////////////////
// Construction
public:
  //! Standard constructor.
	FooButton();

  //! Standard destructor.
  virtual ~FooButton();

/////////////////////////////////////////////////////////////////////////////
// Attributes
public:
  //! Button types
  static enum Type {
    //! Static button
    staticButton = 0,
    //! Standard pushbutton
    pushButton,
    //! Hot tracked pushbutton
    hotPushButton,
    //! Standard pushbutton with dropdown
    pushButtonDropDown,
    //! Hot tracked pushbutton with dropdown
    hotPushButtonDropDown,
    //! Multi pushbutton with dropdown
    pushButtonMulti,
    //! Hot tracked multi pushbutton with dropdown
    hotPushButtonMulti,
    //! Standard check button
    checkButton,
    //! Hot tracked check button
    hotCheckButton,
    //! Hyperlink
    hyperlink,
  };

  //! Text styles
  static enum Text {
    //! No text displayed
    none = 0,
    //! Single-line left-justified text
    singleLine,
    //! Single-line centered text
    singleLineCenter,
    //! Multi-line left-justified text
    multiLine,
  };

/////////////////////////////////////////////////////////////////////////////
// Operations
public:
  //! Gets the button's type.
  //! @return   The button's type.
  FooButton::Type getType()
    { return m_type; }

  //! Sets the button's type.
  //! @param  type    The button's type.
  void setType
    (FooButton::Type type);

  //! Gets the button's text style.
  //! @return   The button's text style.
  FooButton::Text getTextStyle()
    { return m_textStyle; }

  //! Sets the button's text style.
  //! @param  textStyle   The button's text style.
  void setTextStyle
    (FooButton::Text textStyle);

  //! Gets the button's bitmap id.
  //! @return   The button's bitmap id.
  int getBitmapId()
    { return m_nBitmapId; }

  //! Sets the button's bitmap id.
  //! @param  nBitmapId       Bitmap id (-1 means none).
  //! @param  rgbTransparent  Bitmap's transparency color (default = RGB(255,0,255)
  void setBitmapId
    (int nBitmapId,
     COLORREF rgbTransparent = RGB (255, 0, 255));

  //! Gets the button's checked state.
  //! @return   The button's checked state.
  bool isChecked()
    { return m_bChecked; }

  //! Sets the button's checked state if the button type is checkButton or
  //! hotCheckButton.
  //! @param    bChecked    Flag: button is checked.
  //! @return   The button's checked state.
  void check
    (bool bChecked);

  //! Checks if the multi pushbutton's drop-down was clicked.
  //! @return   true if the button's drop-down was clicked, false otherwise.
  bool isMultiClicked()
    { return m_bMultiClicked; }

  //! Sets a multi pushbutton to its normal unpressed state.
  void clearMultiClick();

  //! Displays a popup menu if the button type is pushButton, pushButtonDropDown,
  //! hotPushButton or hotPushButtonDropDown.
  //! @param    pMenu   The popup menu to be displayed.
  //! @return   The menu selection.
  int displayPopupMenu
    (CMenu* pMenu);

/////////////////////////////////////////////////////////////////////////////
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FooButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

/////////////////////////////////////////////////////////////////////////////
// Implementation
	// Generated message map functions
protected:
	//{{AFX_MSG(FooButton)
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
  afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave (WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
  afx_msg void OnKillFocus (CWnd* pNewWnd);
  afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnSetFocus (CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  //! Button type.
  FooButton::Type m_type;

  //! Text style.
  FooButton::Text m_textStyle;

  //! Flag: mouse is being tracked.
  BOOL  m_bTracking;

  //! Bitmap id.
  int m_nBitmapId;

  //! Flag: draw single-line centered text.
  bool m_bCenter;

  //! Flag: draw multi-line text
  bool m_bMultiLine;

  //! Flag: button is checked.
  bool m_bChecked;

  //! Flag: display text.
  bool m_bText;

  //! Flag: display dropdown indicator.
  bool m_bDropDown;

  //! Flag: behave as multi button.
  bool m_bMulti;

  //! Flag: the button's drop-down was clicked.
  bool m_bMultiClicked;

  //! Flag: display focus rectangle.
  bool m_bFocus;

  //! Flag: display as static control.
  bool m_bStatic;

  //! Flag: behave as hot button.
  bool m_bHot;

  //! Flag: display as hyperlink
  bool m_bHyperlink;

  // Bitmap transparency color.
  COLORREF m_rgbTransparent;

  // Hyperlink cursor
  HCURSOR m_hCursor;

/////////////////////////////////////////////////////////////////////////////
// Helper functions
protected:
  //! Draws the button's frame
  void drawFrame
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct);

  //! Draws the button's bitmap
  void drawBitmap
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct,
     int& nBitmapRightEdge);

  //! Draws the button's caption
  void drawCaption
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct,
     int nLeftEdge);

  //! Draws the button's drop-down indicator
  void drawDropDown
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct);

  //! Draws a multi-button's pressed drop-down region
  void drawMultiDropDownRegion
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct);

  //! Draws the button's focus rectangle
  void drawFocus
    (CDC* pDC,
     LPDRAWITEMSTRUCT lpDrawItemStruct);

  //! Draws a bitmap with transparency color.
  //! See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1749/
  void TransparentBlt
    (HDC hdcDest, int nXDest, int nYDest, int nWidth,
     int nHeight, HBITMAP hBitmap, int nXSrc, int nYSrc,
     COLORREF colorTransparent, HPALETTE hPal);

  //! Draws a bitmap as disabled.
  //! See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1699/
  void DisabledBlt
    (HDC hdcDest, int nXDest, int nYDest, int nWidth,
     int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);

  //! Draws the button's drop-down triangle.
  void DrawDropDownIndicator
    (HDC hdcDest, int nX, int nY, int nWidthDropDown, int nHeightDropDown);

  //! Draws the button's frame when hot tracking.
  void drawHotButtonFrame
    (LPDRAWITEMSTRUCT lpDrawItemStruct);

  //! Loads the default hyperlink cursor if possible.
  void loadHyperlinkCursor();

  //! Checks if the mouse was clicked on multi part of button.
  void multiHitTest
    (CPoint pt);
};

#endif

// End FooButton.h