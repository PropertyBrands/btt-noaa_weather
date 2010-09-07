<?php
/**
 * @file admin.noaa.inc
 * NOAA Weather Admin Functions
 * Handles all the settings for the NOAA Weather Module
 * Version: 1.0
 */

/**
 * @name _noaa_weather_check_install
 * This checks the install when the module is initialized on the admin pages
 * @return bool $error return the error status
 */
function _noaa_weather_check_install() {
  $settings_link = l(' Click here for NOAA Weather settings', 'admin/settings/noaaweather');
  $error         = FALSE;
  //Current weather station
  if (variable_get('noaa_weather_current_weather_station_default', 'INIT') == 'INIT') {
    $MSG = t( 'You have not entered a current condition weather station, please select a station from the NOAA Weather Station List.');
    drupal_set_message($MSG, 'error', FALSE);
    $error = TRUE;
  }
  //Current Location
  if (is_null(variable_get('noaa_weather_current_location_name_default', NULL))) {
    $MSG = t( 'You have not named your current location.');
    drupal_set_message($MSG, 'error', FALSE);
    $error = TRUE;
  }
  //Radar station
  if (variable_get('noaa_weather_radar_station_default', 'INIT') == 'INIT') {
    $MSG = t( 'You have not entered a radar station, please select a station from the NOAA Radar Station List.');
    drupal_set_message($MSG, 'error', FALSE);
    $error = TRUE;
  }
  //Module Link
  if ($error == TRUE) {
    $MSG = t( '!link', array('!link' => $settings_link) );
    drupal_set_message($MSG, 'error', FALSE);
  }
}
/**
 * Main settings form
 */
function _noaa_weather_main_settings() {
  //Manually Update All Weather
  $form['main'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('NOAA Weather Manual Update'),
    '#description' => t('Manually Update All Weather through this interface.'),
		'#collapsible' 	=> FALSE,
		'#collapsed' 		=> FALSE,
		'#weight'				=> -50,
    '#group'       => 'main',
  );

  //Update NOAA Weather, this should flag an error when we first start the module.
  $attributes['class'] = 'update-noaa-weather';
  $attributes['query'] = drupal_get_destination();
  $items[] = l('Update All Weather', 'admin/settings/noaaweather/update/all', $attributes);
  $form['main']['updatelink'] = array(
    '#prefix' => '<p>',
    '#value'   => theme('item_list', $items),
    '#suffix'  => '</p>',
    '#group'   => 'main',
  );
  //Enable vertical tabs if installed.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  return $form;
}

function _noaa_weather_current_settings() {
  //Form Settings @todo = what is this for?
  $form['current_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('NOAA Weather Current Condition Settings'),
    '#description' => t('These are settings for your current forecast.'),
		'#collapsible' => FALSE,
		'#collapsed' 	 => FALSE,
		'#weight'			 => -50,
    '#group'       => 'main_settings',
  );
  //Weather station.
  //@todo - make this so we can have multiple locations to choose from
  $form['current_settings']['noaa_weather_current_weather_station_default'] = array(
    '#type'          => 'select',
    '#title'         => t('Current Conditions Station'),
    '#description'   => t('Please select the station you would like to pull data from.'),
    '#multiple'      => FALSE,
    '#options'       => noaaStations::_weatherStations(),
    '#default_value' => NOAA_WEATHER_CURRENT_WEATHER_STATION_DEFAULT,
    '#required'      => TRUE,
    '#group'         => 'main_settings',
  );
  //Location name.
  $form['current_settings']['noaa_weather_current_location_name_default'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Current Conditions Location Name'),
    '#description'   => t('Please enter in the name for this location.'),
    '#default_value' => NOAA_WEATHER_CURRENT_LOCATION_NAME_DEFAULT,
    '#required'      => TRUE,
    '#group'         => 'main_settings',
  );
  //Temperature unit.
  $temp_name = array(
    'f' => t('Farenheit'),
    'c' => t('Centigrade'),
    'b' => t('Both')
  );
  $form['current_settings']['noaa_weather_current_temp_unit_default'] = array(
    '#type'          => 'select',
    '#title'         => t('Default Temperature Unit'),
    '#description'   => t('Please select the default temperature unit you wish to use.'),
    '#multiple'      => FALSE,
    '#options'       => $temp_name,
    '#default_value' => NOAA_WEATHER_CURRENT_TEMP_UNIT_DEFAULT,
    '#required'      => TRUE,
    '#group'         => 'main_settings',
  );
  //Enable vertical tabs if installed.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  return system_settings_form( $form );
}

//Submit Current Settings Form
function _noaa_weather_current_settings_submit($form_id, &$form_state) {
  $update_message = t('Your NOAA Weather current condition settings have been saved.');
  drupal_set_message($update_message);
}


//Forecast Admin Form
function _noaa_weather_forecast_settings() {
	//Point forecast main options
  //@todo - where do the lat and lon come from?
	$form['point_forecast'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('NOAA Weather Point Forecast Settings'),
		'#description' => t('These are the settings for forecasts from NOAA.'),
		'#weight'      => -50,
    '#group'       => 'point_forecast',
  );
  //Latitude
  $form['point_forecast']['noaa_weather_forecast_lat_default'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Latitude'),
    '#default_value' => NOAA_WEATHER_FORECAST_LAT_DEFAULT,
    '#size'          => 6,
    '#weight'        => -50,
    '#group'         => 'point_forecast',
  );
  //Longitude
  $form['point_forecast']['noaa_weather_forecast_lon_default'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Longitude'),
    '#default_value' => NOAA_WEATHER_FORECAST_LON_DEFAULT,
    '#size'          => 6,
    '#weight'        => -49,
    '#group'         => 'point_forecast',
  );
  //Forecast Days
  $forecast_days = array(
    '1' => t('1'),
    '2' => t('2'),
    '3' => t('3'),
    '4' => t('4'),
    '5' => t('5'),
    '6' => t('6'), 
    '7' => t('7'),
    '8' => t('8'),
    '9' => t('9'),
  );
  $form['point_forecast']['noaa_weather_forecast_days_default'] = array(
    '#type'          => 'select',
    '#title'         => t('Number of Day to Forecast'),
    '#default_value' => NOAA_WEATHER_FORECAST_DAYS_DEFAULT,
    '#options'       => $forecast_days,
    '#weight'        => -48,
    '#group'         => 'point_forecast',
  );
  //Day Span
  $day_span = array(
    '12' => t('12 Hour'),
    '24' => t('24 Hour'),
  );
  $form['point_forecast']['noaa_weather_forecast_dayspan_default'] = array(
    '#type'          => 'select',
    '#title'         => t('Forecast Span'),
    '#description'   => t('Display 12 or 24 hour forecast.'),
    '#default_value' => NOAA_WEATHER_FORECAST_DAYSPAN_DEFAULT,
    '#options'       => $day_span,
    '#weight'        => 1,
    '#group'         => 'point_forecast',
   );
  //Temp Name
  $temp_name = array(
    'f' => t('Farenheit'),
    'c' => t('Centigrade'),
    'b' => t('Both')
  );
  $form['point_forecast']['noaa_weather_forecast_temp_unit_default'] = array(
    '#type'          => 'select',
    '#title'         => t('Tempature Unit'),
    '#description'   => t('Select temperature units to display.'),
    '#default_value' => NOAA_WEATHER_FORECAST_TEMP_UNIT_DEFAULT,
    '#options'       => $temp_name,
    '#weight'        => 2,
    '#group'         => 'point_forecast',
  );
  //Enable vertical tabs if installed.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  return system_settings_form( $form );
}

//Validate future forecast.
function _noaa_weather_forecast_settings_validate($form_id, &$form_state) {
  //if we have an invalide lat or lon, set the error.
  if (!is_numeric($form_state['values']['noaa_weather_forecast_lat_default']) || $form_state['values']['noaa_weather_forecast_lat_default'] > 90 || $form_state['values']['noaa_weather_forecast_lat_default'] < -90) {
    form_set_error('noaa_weather_forecast_lat_default', t('Latitude value must be numeric and between 90 and -90.'));
  }
  if (!is_numeric($form_state['values']['noaa_weather_forecast_lon_default']) || $form_state['values']['noaa_weather_forecast_lon_default'] > 180 || $form_state['values']['noaa_weather_forecast_lon_default'] < -180) {
    form_set_error('noaa_weather_forecast_lon_default', t('Longitude value must be numeric and between 180 and -180.'));
  }
}

//Submit Forecast.
function _noaa_weather_forecast_settings_submit($form_id, &$form_state) {
  $update_message = t('NOAA Forecast settings have been saved.');
  drupal_set_message($update_message);
}

//Radar Settings.
function _noaa_weather_radar_settings() {	
	$form['radar'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('NOAA Weather Radar Settings'),
		'#description' => t('These are the settings for radar images from NOAA.'),
		'#collapsible' => FALSE,
		'#collapsed' 	 => FALSE,
		'#weight'      => -50,
    '#group'       => 'radar',
  );  
	$form['radar']['noaa_weather_radar_station_default'] = array(
		'#type'          => 'select',
		'#title'         => t('NOAA Ridge Radar Station'),
		'#multiple'      => FALSE,
		'#options'       => noaaStations::_radarStations(),
		'#description'   => t(''),
		'#weight'        => 1,
		'#default_value' => NOAA_WEATHER_RADAR_STATION_DEFAULT,
    '#group'         => 'radar',
	);
	$form['radar']['noaa_weather_radar_width_default'] = array(
		'#type'          => 'textfield',
		'#title'         => t('Ridge Radar Picture Width (in pixels)'),
		'#description'   => t(''),
		'#size'          => 10,
		'#weight'        => 2,
		'#default_value' => NOAA_WEATHER_RADAR_PICTURE_WIDTH_DEFAULT,
    '#group'         => 'radar',
	);
	$form['radar']['noaa_weather_radar_height_default'] = array(
		'#type'          => 'hidden',
		'#weight'        => 3,
		'#default_value' => NOAA_WEATHER_RADAR_PICTURE_HEIGHT_DEFAULT,
    '#group'         => 'radar',
	);
	$form['radar']['noaa_weather_radar_height_display'] = array(
		'#value'  => t('!example', array( '!example' => '<strong>Ridge Radar Picture Height (in pixels)</strong><div id="radar-height">' .NOAA_WEATHER_RADAR_PICTURE_HEIGHT_DEFAULT .'px</div>')),
    '#weight' => 4,
    '#group'  => 'radar',
	);	  
  $form['radar']['how'] = array(
    '#value'  => t( '!info', array( '!info' => '<p>The actual pixel dimensions of radar images and overlays, as downloaded from NOAA are 600px X 550px. One can adjust the width dimension to allow the radar picture to fit in a specific layout. Simply change the width above to the desired target in pixels. The height will adjust automatically at a ratio of approximately .9166 to 1 on saving. Note! - We are not changing the actual width and height of the downloaded images from NOAA. Rather we are specifying the height and width attributes of the images and the associated css. Hence, adjusting to a smaller widths may result in the text in overlays such as the cities, radar warnings , and legend being illegible. The clarity thresh-hold is approximately 400px in width.</p>' )),
    '#weight' => 5,
    '#group'  => 'radar',
  );
  //Enable vertical tabs if installed.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
	return system_settings_form( $form );
}

function _noaa_weather_radar_settings_validate($form_id, &$form_state) {
  if(NOAA_WEATHER_RADAR_STATION_DEFAULT != $form_state['values']['noaa_weather_radar_station_default']) {
    //If our radar stations are different, get the new radar station
    if (_noaa_weather_get_radar_overlays($form_state['values']['noaa_weather_radar_station_default'])) {
      $update_message .= t('Radar Images for Station "!rad" updated', array('!rad'=>strtoupper($form_state['values']['noaa_weather_radar_station_default'])));
      drupal_set_message($update_message);
    }
  }
}

function _noaa_weather_radar_settings_submit($form_id, &$form_state) {
  $update_message = t('NOAA Radar settings have been saved');
  drupal_set_message($update_message);
}

//Icon Settings
function _noaa_weather_icon_settings() {
  //@todo - I would like to make these all part of a define() in another .inc file
	$HELP = t('Please choose your custom directory first. After you have chosen your custom directory, you will be able to assign a custom icon to ecah NOAA default image, or reset it to the default image. After you finish assign images for each inidivual page, you must press "Save configuration" before you move to the next page.');
  $form['icons'] = array(
		'#type'        => 'fieldset',
		'#title'       => t('NOAA Weather Icon Settings'),
    '#description' => $HELP,
		'#collapsible' => FALSE,
		'#collapsed' 	 => FALSE,
		'#weight'      => -50,
    '#group'       => 'icons',
  );
  //If we have folders in the custom icon directory, then we need to populate the icons
  $icon_select_list = '';
  $custom_icons     = file_scan_directory(NOAA_WEATHER_CUSTOM_IMAGE_DIRECTORY, '.jpg|.jpeg|.gif|.png|.bmp');
  //Choose you icon pack and set the icons
  if (!empty($custom_icons)) {
    $icons            = array('INIT' => t('Choose a custom icon'));
    $directories      = array('INIT' => t('Choose a custom directory'));
    //We have to change this array and turn it into a directory structure
    foreach ($custom_icons as $file) {
      $search  = array(NOAA_WEATHER_CUSTOM_IMAGE_DIRECTORY .'/', '/' .$file->basename);
      $replace = array('','');
      $dir     = str_replace($search, $replace, $file->filename);
      $icons[$dir][$file->filename] = $file->basename;
      $directories[NOAA_WEATHER_DEFAULT_IMAGE_DIRECTORY .'/' .$dir] = $dir;
      $icon_select_list .= '<option value="'.$file->filename.'" title="'. url($file->filename).'" >' .$file->basename .'</option>';
    }
    //Make the form from the icon directory
    $form['icons']['noaa_weather_icons_custom_directories'] = array(
      '#type'          => 'select',
      '#title'         => t('Select a Directory to use as your custom icons.'),
      '#multiple'      => FALSE,
      '#options'       => $directories,
      '#weight'        => -50,
      '#default_value' => variable_get('noaa_weather_icons_custom_directories', 'INIT'),
      '#group'         => 'icons',
    );
  }
  //If we have icons, let us chose to customize them
  if (variable_get('noaa_weather_icons_custom_directories', 'INIT') != 'INIT') {
    //Get the images from the noaaImages class, and turn them into form elements
    $defaultImages  = noaaImages::_defaultImages();
    $items_per_page = 20;
    $imageSets      = array_chunk($defaultImages, $items_per_page, TRUE);
    //Sneaky paging gleened from: http://acquia.com/blog/sneaky-drupal-pagers
    $GLOBALS['pager_page_array'][] = isset($_GET['page']) ? $_GET['page'] : 0; //what page you are on
    $GLOBALS['pager_total'][]      = count($imageSets); // total number of pages
    //Form For Pager
    $form['icons']['pager'] = array(
      '#value'  => theme('pager', NULL, $items_per_page),
      '#weight' => -49,
    );
    //Loop through sets, and only show the set based on the paging array
    $image_list = variable_get('noaa_weather_icons_list', NULL);
    foreach ($imageSets as $page => $set) {
      //If our set matches the paging array value
      if ($GLOBALS['pager_page_array'][0] == $page ) {
        foreach ($set as $key => $value) {
          //Switch out images for their custom icon if we have one
          $default_image = NOAA_WEATHER_DEFAULT_IMAGE_DIRECTORY .$value;
          $image         = (image_get_info($image_list[$default_image])) ? $image_list[$default_image] : $default_image;
          if (image_get_info($image)) {
            //Put the default image in the select list as well
            $default_image_option = '<option value="'.$default_image.'" title="'. url($default_image).'" >' .t('Default Image') .'</option>';
            //Make the Hidden field with default image and custom image dropdown
            $form['icons']['pager']['noaa_weather_icons_default'][$default_image] = array(
              '#type'          => 'hidden',
              '#weight'        => $key,
              '#default_value' => $image,
              '#prefix'        => '<div class="custom-icon-wrapper">',
              '#suffix'        => '<select id="custom-icons-'. $key .'" class="custom-icons"><option value="'.$image.'" title="'. url($image).'" selected="selected" >' .t('Current Image') .''. $default_image_option. $icon_select_list .'</select></div>',
              '#group'         => 'icons',
            );
          }
        }
      }
    }
  }
  //Enable vertical tabs if installed.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  //Add our own submit handler after the system sets its variables
  $form['#submit'][] = '_noaa_weather_icon_settings_submit';
  return system_settings_form($form);
}

function _noaa_weather_icon_settings_validate($form_id, &$form_state){
  //Switch between ops, in case we decide to reset
  switch ($form_state['values']['op']) {
    case 'Reset to defaults':
      //If we have decided to reset everything to the defaults - you are an idiot if you have spent the time to customize the icons
      $defaultImages  = noaaImages::_defaultImages();
      $images = array();
      foreach ($defaultImages as $key => $value) {
        //We want to loop through the default images, and reset the image array, replacing the images with the custom values
        $image = NOAA_WEATHER_DEFAULT_IMAGE_DIRECTORY .$value;
        $images[$image] = $image;
      }
      variable_set('noaa_weather_icons_list', $images);
      //Return A friendly message, when we reset
      $MSG = t('Images hve been reset to their defaults.');
      drupal_set_message($MSG);
      break;
    default:
      //Set the noaa_weather_icons_list with the new values
      $image_list = variable_get('noaa_weather_icons_list', NULL);
      foreach ($image_list as $key => $value) {
        //We want to loop through the default images, and reset the image array, replacing the images with the custom values - we have to convert the image list keys to match the post key so our arrays match - stupid drupal
        $find    = array('.jpg','.jpeg','.bmp','.gif','.png');
        $replace = array('_jpg','_jpeg','_bmp','_gif','_png');
        $posted_key = str_replace($find,$replace,$key);
        if ($form_state['clicked_button']['#post'][$posted_key] != NULL && $form_state['clicked_button']['#post'][$posted_key] != $value) {
          $image_list[$key] = $form_state['clicked_button']['#post'][$posted_key];
        }
      }
      variable_set('noaa_weather_icons_list', $image_list);
      //Return A friendly message, when we save
      $MSG = t('Custom images have been updated.');
      drupal_set_message($MSG);
      break;
  }
}

function _noaa_weather_icon_settings_submit($form_id, &$form_state){
  //because Drupal sucks at redirecting forms with our hacked pagers, we need to get the paging info
  $query = '';
  if ($_GET['page']) { $query = 'page=' .$_GET['page']; }
  drupal_goto($_GET['q'],$query);
}