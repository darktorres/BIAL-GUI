#include "controller.h"
#include "imageviewer.h"
#include "imagewidget.h"

#include <Common.hpp>
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGridLayout>

#include <iostream>

ImageViewer::ImageViewer( QWidget *parent ) : QWidget( parent ) {
  for( size_t i = 0; i < views.size( ); ++i ) {
    views[ i ] = new ImageWidget( this );
    views[ i ]->hideControls( );
    views[ i ]->scene( )->installEventFilter( this );
    views[ i ]->setViewNumber( i );
  }
  layout = new QGridLayout( this );
  layout->setVerticalSpacing( 10 );
  layout->setHorizontalSpacing( 10 );
  setGridLayout( );

}

ImageViewer::~ImageViewer( ) {

}

void ImageViewer::setBackgroundColor( const QColor &color ) {
  for( ImageWidget *view : views ) {
    view->setBackgroundColor( color );
  }
}

void ImageViewer::setController( Controller *value ) {
  controller = value;
  connect( controller, &Controller::imageChanged, this, &ImageViewer::changeImage );
  connect( controller, &Controller::imageUpdated, this, &ImageViewer::updateImage );
  for( ImageWidget *view : views ) {
    connect( view, &ImageWidget::sliceChanged, controller, &Controller::setCurrentSlice );
  }
}

void ImageViewer::updateViews( ) {
  for( size_t i = 0; i < 4; ++i ) {
    views[ i ]->fitInView( );
  }
}

void ImageViewer::updateImage( ) {
  COMMENT( "ImageViewer::updateImage", 2 );
  GuiImage *img = controller->currentImage( );
  if( !img ) {
    return;
  }
  for( size_t i = 0; i < 4; ++i ) {
    views[ i ]->setSlice( img->currentSlice( i ) );
  }
}

void ImageViewer::changeImage( ) {
  COMMENT( "ImageViewer::changeImage", 0 );
  GuiImage *img = controller->currentImage( );
  if( !img ) {
    return;
  }
  DisplayFormat format = controller->currentFormat( );
  for( size_t i = 0; i < 4; ++i ) {
    if(format.viewerControls){
      views[ i ]->setRange( 0, img->depth( i ) - 1 );
      views[ i ]->setSlice( img->currentSlice( i ) );
      views[ i ]->showControls( );
    }else{
      views[ i ]->hideControls( );
    }
  }
  setLayout( format.currentLayout );
  setViewMode( format.currentViews );
  /* TODO Continue ... */
}

void ImageViewer::setLayout( Layout layout ) {
  switch( layout ) {
      case Layout::GRID:
      setGridLayout( );
      break;
      case Layout::HORIZONTAL:
      setHorizontalLayout( );
      break;
      case Layout::VERTICAL:
      setVerticalLayout( );
      break;
      case Layout::NONE:
      break;
  }
}

void ImageViewer::setGridLayout( ) {
  layout->addWidget( views[ 0 ], 0, 0 );
  layout->addWidget( views[ 1 ], 0, 1 );
  layout->addWidget( views[ 2 ], 1, 0 );
  layout->addWidget( views[ 3 ], 1, 1 );
  updateViews( );
}

void ImageViewer::setHorizontalLayout( ) {
  layout->addWidget( views[ 0 ], 0, 0 );
  layout->addWidget( views[ 1 ], 0, 1 );
  layout->addWidget( views[ 2 ], 0, 2 );
  layout->addWidget( views[ 3 ], 0, 3 );
  updateViews( );
}

void ImageViewer::setVerticalLayout( ) {
  layout->addWidget( views[ 0 ], 0, 0 );
  layout->addWidget( views[ 1 ], 1, 0 );
  layout->addWidget( views[ 2 ], 2, 0 );
  layout->addWidget( views[ 3 ], 3, 0 );
  updateViews( );
}

void ImageViewer::hideViews( ) {
  for( ImageWidget *view : views ) {
    view->hide( );
  }
}

void ImageViewer::showViews( ) {
  for( ImageWidget *view : views ) {
    view->show( );
  }
}

void ImageViewer::setViewMode( Views views ) {
  switch( views ) {
      case Views::NONE:
      hideViews( );
      break;
      case Views::SHOW0:
      setView0( );
      break;
      case Views::SHOW1:
      setView1( );
      break;
      case Views::SHOW2:
      setView2( );
      break;
      case Views::SHOW3:
      setView3( );
      break;
      case Views::SHOW012:
      setViews012( );
      break;
      case Views::SHOW123:
      setViews123( );
      break;
      case Views::SHOW0123:
      setViews0123();
      break;
  }
}

void ImageViewer::setView0( ) {
  hideViews( );
  views[ 0 ]->show( );
}

void ImageViewer::setView1( ) {
  hideViews( );
  views[ 1 ]->show( );
}

void ImageViewer::setView2( ) {
  hideViews( );
  views[ 2 ]->show( );
}

void ImageViewer::setView3( ) {
  hideViews( );
  views[ 3 ]->show( );
}

void ImageViewer::setViews012( ) {
  showViews( );
  views[ 3 ]->hide( );
}

void ImageViewer::setViews123( ) {
  showViews( );
  views[ 0 ]->hide( );
}

void ImageViewer::setViews0123( ) {
  showViews( );
}

bool ImageViewer::eventFilter( QObject *obj, QEvent *evt ) {
  QGraphicsSceneMouseEvent *mouseEvt = dynamic_cast< QGraphicsSceneMouseEvent* >( evt );
  size_t scene = 0;
  for( size_t scn = 1; scn < views.size( ); ++scn ) {
    if( obj == views[ scn ]->scene( ) ) {
      scene = scn;
    }
  }
  if( mouseEvt ) {
    emit updateStatus( QString( "Scene %3 position: (%1, %2)" ).arg( mouseEvt->scenePos( ).x( ) ).arg(
                         mouseEvt->scenePos( ).y( ) ).arg( scene ), 100 );
  }
  return( QWidget::eventFilter( obj, evt ) );
}

QGraphicsScene* ImageViewer::getScene( size_t axis ) {
  if( axis > views.size( ) ) {
    throw std::out_of_range( BIAL_ERROR( QString( "Invalid axis, expected < %1." ).arg( views.size( ) ).toStdString( ) ) );
  }
  return( views[ axis ]->scene( ) );
}
