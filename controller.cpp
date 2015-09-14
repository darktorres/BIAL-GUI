#include "controller.h"
#include "thumbswidget.h"

Controller::Controller( int views, QObject *parent ) : QObject( parent ) {
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
  return( true );
}

bool Controller::addLabel( QString label ) {

}

bool Controller::removeCurrentLabel( ) {

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

}

void Controller::setCurrentImagePos( int position ) {
  m_currentImagePos = position;
  if( currentImage( ) ) {
    disconnect( currentImage( ), &GuiImage::imageUpdated, this, &Controller::update );
  }
  m_currentImagePos = position;
  update( );
  if( currentImage( ) ) {
    connect( currentImage( ), &GuiImage::imageUpdated, this, &Controller::update );
  }
  emit imageChanged( );
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

void Controller::setThumbsWidget( ThumbsWidget *thumbsWidget ) {
  m_thumbsWidget = thumbsWidget;
  m_thumbsWidget->setController( this );
}
