#include "controller.h"
#include "thumbswidget.h"

#include <QFile>
#include <qsettings.h>

Controller::Controller( int views, QObject *parent ) : QObject( parent ), noneFormat( Modality::NONE ), bwFormat(
    Modality::BW ), rgbFormat( Modality::RGB ), niftiFormat( Modality::NIfTI ) {
  for( int item = 0; item < views; ++item ) {
    m_pixmapItems.append( new PixmapLabelItem( ) );
  }
  m_currentImagePos = -1;
}

GuiImage* Controller::currentImage( ) {
  return( imageAt( currentImagePos( ) ) );
}

GuiImage* Controller::imageAt( int pos ) {
  if( ( pos >= 0 ) && ( pos < m_images.size( ) ) ) {
    return( m_images.at( pos ) );
  }
  return( nullptr );
}

int Controller::currentImagePos( ) const {
  return( m_currentImagePos );
}

bool Controller::addImage( QString fname ) {
  COMMENT( "Loading file " << fname.toStdString( ), 0 );
  GuiImage *img = nullptr;
  try {
    img = new GuiImage( fname, this );
  }
  catch( std::bad_alloc e ) {
    BIAL_WARNING( e.what( ) );
  }
  catch( std::runtime_error e ) {
    BIAL_WARNING( e.what( ) );
  }
  catch( std::out_of_range e ) {
    BIAL_WARNING( e.what( ) );
  }
  catch( std::logic_error e ) {
    BIAL_WARNING( e.what( ) );
  }
  catch( ... ) {

  }
  if( img == nullptr ) {
    return( false );
  }
  m_images.append( img );

  emit containerUpdated( );

  m_thumbsWidget->addThumbnail( img );
  if( currentImagePos( ) == -1 ) {
    setCurrentImagePos( 0 );
  }
  setRecentFile( fname );

  return( true );
}

bool Controller::addLabel( QString label ) {
  /* TODO Controller::addLabel( QString label ) */
}

bool Controller::removeCurrentLabel( ) {
  /* TODO Controller::removeCurrentLabel( ) */
}

void Controller::removeCurrentImage( ) {
  m_images.removeAt( currentImagePos( ) );
  m_thumbsWidget->removeAt( currentImagePos( ) );
  emit containerUpdated( );
  if( currentImagePos( ) == 0 ) {
    setCurrentImagePos( 0 );
  }
  else {
    setCurrentImagePos( currentImagePos( ) - 1 );
  }
}

bool Controller::isEmpty( ) {
  return( m_images.isEmpty( ) );
}

void Controller::clear( ) {
  COMMENT( "Reseting images.", 1 );
  qDeleteAll( m_images );
  m_images.clear( );
  setCurrentImagePos( -1 );
  m_thumbsWidget->clear( );
  emit containerUpdated( );
}

int Controller::size( ) {
  return( m_images.size( ) );
}

void Controller::update( ) {
  COMMENT( "UPDATING IMAGE!", 1 );

  GuiImage *img = currentImage( );
  if( img ) {
    int items = 1;
    if( img->modality( ) == Modality::NIfTI ) {
      items = 3;
    }
    else if( img->modality( ) == Modality::RGB ) {
      items = 4;
    }
    for( int axis = 0; axis < items; ++axis ) {

      const QPixmap &pix = img->getSlice( axis, img->currentSlice( axis ) ); /* .scaledToHeight(img->heigth(axis) *
                                                                              * scale); */
      m_pixmapItems.at( axis )->setImage( pix );

      /* TODO Label rendering. */

      /*
       *      if( img->currentLabel( ) != NULL ) {
       *        m_pixmapItems[ axis ]->setLabel( img->currentLabel( )->getLabel( axis,
       *                                                                         img->currentSlice(
       *                                                                           axis ) ) );
       *      }
       *      else {
       *        m_pixmapItems[ axis ]->setLabel( QPixmap( ) );
       *      }
       */
    }
  }
  else {
    for( int axis = 0; axis < m_pixmapItems.size( ); ++axis ) {
      m_pixmapItems[ axis ]->setImage( QPixmap( ) );
      m_pixmapItems[ axis ]->setLabel( QPixmap( ) );
    }
  }
  emit imageUpdated( );
}

void Controller::setCurrentImagePos( int position ) {
  m_currentImagePos = position;
  if( currentImage( ) != nullptr ) {
    disconnect( currentImage( ), &GuiImage::imageUpdated, this, &Controller::update );
  }
  if( currentImage( ) != nullptr ) {
    emit imageChanged( );
    update( );
    connect( currentImage( ), &GuiImage::imageUpdated, this, &Controller::update );
  }
}

void Controller::loadNextImage( ) {
  if( currentImagePos( ) == ( m_images.count( ) - 1 ) ) {
    setCurrentImagePos( 0 );
  }
  else {
    setCurrentImagePos( currentImagePos( ) + 1 );
  }
}

void Controller::changeOthersSlices( QPointF posF, int axis ) {

}

void Controller::setCurrentSlice( size_t axis, size_t slice ) {
  currentImage( )->setCurrentSlice( axis, slice );
}

void Controller::setZoom( int value ) {
  /* FIXME Zoom doesn't work yet. */
  scale = 1.0 + value / 100.0;
  update( );
}

void Controller::setRecentFile( QString fname ) {
  COMMENT( "Setting recent file to : \"" << fname.toStdString( ) << "\"", 1 );
  if( !QFile( fname ).exists( ) ) {
    return;
  }
  QSettings settings;
  QStringList files = settings.value( "recentFileList" ).toStringList( );

  files.removeAll( fname );

  files.prepend( fname );
  while( files.size( ) > MaxRecentFiles ) {
    files.removeLast( );
  }
  settings.setValue( "recentFileList", files );

  emit recentFilesUpdated( );

}

void Controller::setThumbsWidget( ThumbsWidget *thumbsWidget ) {
  m_thumbsWidget = thumbsWidget;
  m_thumbsWidget->setController( this );
}

DisplayFormat & Controller::currentFormat( ) {
  if( !currentImage( ) ) {
    return( noneFormat );
  }
  Modality mod = currentImage( )->modality( );
  switch( mod ) {
  case Modality::NONE: {
      return( noneFormat );
      break;
    }
      case Modality::BW: {
      return( bwFormat );
      break;
    }
      case Modality::RGB: {
      return( rgbFormat );
      break;
    }
      case Modality::NIfTI: {
      return( niftiFormat );
      break;
    }
  }
  return( noneFormat );
}

PixmapLabelItem* Controller::getPixmapItem( size_t axis ) {
  return( m_pixmapItems.at( axis ) );
}
