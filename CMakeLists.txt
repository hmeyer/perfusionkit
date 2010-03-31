CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( perfusionkit )

MESSAGE( "VTK_DIR: ${VTK_DIR}" )
FIND_PACKAGE ( VTK )
IF ( VTK_FOUND )
      INCLUDE( ${USE_VTK_FILE} )
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF( VTK_FOUND )

FIND_PACKAGE ( ITK )
IF ( ITK_FOUND )
      INCLUDE( ${USE_ITK_FILE} )
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF( ITK_FOUND )

# use what QVTK built with
SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
SET(DESIRED_QT_VERSION ${VTK_DESIRED_QT_VERSION} CACHE FILEPATH "")
FIND_PACKAGE(Qt)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

IF(NOT DESIRED_QT_VERSION MATCHES 4)
	MESSAGE(FATAL_ERROR "Need QT Version >= 4 build into VTK")
ENDIF(NOT DESIRED_QT_VERSION MATCHES 4)


                           

INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  )
ADD_EXECUTABLE( perfusionkit interactorstylevolumeview.cpp main.cpp multiplanarreformatwidget.cpp vtkinteractorstyleprojectionview.cpp volumeprojectionwidget.cpp)
TARGET_LINK_LIBRARIES ( perfusionkit
  QVTK
  ${QT_LIBRARIES}
  ITKBasicFilters ITKCommon ITKIO
  vtkRendering vtkGraphics vtkHybrid
  vtkImaging vtkIO vtkFiltering vtkCommon
  vtkVolumeRendering
  )

