CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( perfusionkit )



SET( PerfusionkitSrc 
	main/main.cpp 
	main/mainwindow.cpp 
	main/dicomselectordialog.cpp

	analyse/analysedialog.cpp
	analyse/gammaVariate.cpp
	analyse/segmentlistmodel.cpp 
	analyse/segmentinfo.cpp 
	analyse/segmentselectbutton.cpp 
	analyse/timedensitydata.cpp
	analyse/gammafitdata.cpp
	analyse/patlakdata.cpp
	analyse/linearregressiondata.cpp
	analyse/timedensitydatapicker.cpp
	analyse/clearancecalculator.cpp

	imagetree/ctimagetreemodel.cpp
	imagetree/segmentationvalues.cpp
	imagetree/ctimagetreeitem.cpp 
	imagetree/binaryimagetreeitem.cpp 
	imagetree/watershedsegmenttreeitem.cpp 
	imagetree/itkvtktreeitem.cpp 
	imagetree/treeitem.cpp
	imagetree/vtkconnectordatabase.cpp
	imagetree/dicomtagtype.cpp

	serialization/ctimagetreemodel_serializer.cpp 

	vtkwidgets/vtkbinaryimagetocolor.cpp 
	vtkwidgets/vtkbinaryimageoverlay.cpp 
	vtkwidgets/interactorstylevolumeview.cpp 
	vtkwidgets/multiplanarreformatwidget.cpp 
	vtkwidgets/vtkinteractorstyleprojectionview.cpp 
	vtkwidgets/volumeprojectionwidget.cpp 
)

SET( HeaderDirs
	main
	analyse
	imagetree
	serialization
	misc
	vtkwidgets
)

SET( UIS 
	main/MainWindow.ui 
	main/DicomSelectDialog.ui
	analyse/AnalyseDialog.ui
)

SET ( QVTKMocHeaders
	main/mainwindow.h
	main/dicomselectordialog.h
	analyse/segmentselectbutton.h
	analyse/analysedialog.h
	imagetree/ctimagetreemodel.h
    )




#FIND_PATH(MYLIBS itkbasics.h ../myLibs ../../myLibs)

FIND_PATH(QWT_INCLUDE_DIR qwt.h)
FIND_PATH(QWT_LIBRARY_DIR libqwt.so)
IF(WIN32)
  SET(QWT_LIBRARIES qwt5)
ELSE(WIN32)
  SET(QWT_LIBRARIES qwt)
ENDIF(WIN32)


FIND_PACKAGE ( VTK )
IF ( VTK_FOUND )
      INCLUDE( ${VTK_USE_FILE} )
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF( VTK_FOUND )

FIND_PACKAGE ( ITK )
IF ( ITK_FOUND )
      INCLUDE( ${ITK_USE_FILE} )
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF( ITK_FOUND )

FIND_PACKAGE ( GDCM REQUIRED )
INCLUDE( ${GDCM_USE_FILE} )

IF(WIN32)
  SET(GDCM_LIBRARY_DIRS "${GDCM_DIR}/bin")
  SET(GDCM_LIBRARIES gdcmcharls.lib gdcmCommon.lib gdcmDICT.lib
	gdcmDSED.lib gdcmexpat.lib gdcmgetopt.lib gdcmIOD.lib gdcmjpeg8.lib 
	gdcmjpeg12.lib gdcmjpeg16.lib gdcmMSFF.lib gdcmopenjpeg.lib gdcmzlib.lib)
ELSE(WIN32)
ENDIF(WIN32)


SET(Boost_USE_STATIC_LIBS   ON)
SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.40.0 REQUIRED COMPONENTS filesystem signals serialization iostreams date_time system)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

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

ADD_DEFINITIONS(-ftemplate-depth-100)

INCLUDE_DIRECTORIES(
	${HeaderDirs}
  	${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	${QWT_INCLUDE_DIR}
)

LINK_DIRECTORIES(
  ${Boost_LIBRARY_DIRS}
  ${QWT_LIBRARY_DIR}
  ${GDCM_LIBRARY_DIRS}
)

QT4_WRAP_UI( PerfusionkitSrc ${UIS} )


QT4_WRAP_CPP( PerfusionkitSrc ${QVTKMocHeaders} )

                         

ADD_EXECUTABLE( perfusionkit ${PerfusionkitSrc} ${MOCSrcs})
TARGET_LINK_LIBRARIES ( perfusionkit
  QVTK
  ${QT_LIBRARIES}
  ITKBasicFilters ITKCommon ITKAlgorithms ITKIO
  vtkRendering vtkGraphics vtkHybrid
  vtkImaging vtkIO vtkFiltering vtkCommon
  vtkVolumeRendering
  ${GDCM_LIBRARIES}
  ${Boost_LIBRARIES}
  ${QWT_LIBRARIES}
  )

