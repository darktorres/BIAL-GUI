#ifndef DISPLAYFORMAT_H
#define DISPLAYFORMAT_H

#include <QColor>
#include <QObject>
#include <QVector>
#include <array>

enum class Modality {
  BW, RGB, NIfTI
};
enum class Layout {
  GRID, HORIZONTAL, VERTICAL
};
enum class Views {
  SHOW0 = 1, SHOW1 = 2, SHOW2 = 4, SHOW3 = 8, SHOW012 = 7, SHOW123 = 14, SHOW0123 = 15
};

class DisplayFormat : public QObject {
  Q_OBJECT
public:
  explicit DisplayFormat( QObject *parent );

  Modality modality( ) const;

  Layout currentLayout( ) const;
  virtual void setCurrentLayout( const Layout &currentLayout );

  Views currentViews( ) const;
  virtual void setCurrentViews( const Views &currentViews );

  bool hasViewerControls( ) const;

  bool enableTools( ) const;

  bool rotateAll( ) const;

  bool rotateSingle( ) const;

  bool hasOverlay( ) const;

  bool showNiftiViews( ) const;

  bool showNiftiAxis( ) const;

  bool showOrientation( ) const;

  bool showPpmViews( ) const;

  bool showPpmChannels( ) const;

  bool overlay( ) const;

  void setOverlay( bool overlay );

  void toggleOverlay( );

  bool hasLayout( ) const;

  bool has3Views( ) const;

  bool has4Views( ) const;

  std::array< bool, 4 > getViews( );

  int getNumberOfViews( ) const;
  virtual void setNumberOfViews( int numberOfViews ) = 0;

  QColor overlayColor() const;
  void setOverlayColor(const QColor & overlayColor);

signals:
  void updated( );

protected:
  Modality m_modality;
  Layout m_currentLayout;
  Views m_currentViews;

  bool m_overlay;

  bool m_hasViewerControls;
  bool m_enableTools;
  bool m_rotateAll;
  bool m_rotateSingle;

  bool m_hasOverlay;
  bool m_hasLayout;
  bool m_has3Views;
  bool m_has4Views;

  bool m_showNiftiViews;
  bool m_showNiftiAxis;
  bool m_showOrientation;
  bool m_showPpmViews;
  bool m_showPpmChannels;

  int m_numberOfViews;
  QColor m_overlayColor;
};

class BWFormat : public DisplayFormat {
public:
  BWFormat( QObject *parent = 0 );
  void setNumberOfViews( int numberOfViews );
};

class NIfTIFormat : public DisplayFormat {
public:
  NIfTIFormat( QObject *parent = 0 );
  virtual void setCurrentViews( const Views &currentViews );
  void setNumberOfViews( int numberOfViews );
};

class RGBFormat : public DisplayFormat {
public:
  RGBFormat( QObject *parent = 0 );
  void setNumberOfViews( int numberOfViews );
};


#endif /* DISPLAYFORMAT_H */
