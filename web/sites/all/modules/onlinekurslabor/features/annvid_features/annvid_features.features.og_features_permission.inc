<?php
/**
 * @file
 * annvid_features.features.og_features_permission.inc
 */

/**
 * Implements hook_og_features_default_permissions().
 */
function annvid_features_og_features_default_permissions() {
  $permissions = array();

  // Exported og permission: 'node:content_domain:create annvid content'
  $permissions['node:content_domain:create annvid content'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  // Exported og permission: 'node:content_domain:delete any annvid content'
  $permissions['node:content_domain:delete any annvid content'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  // Exported og permission: 'node:content_domain:delete own annvid content'
  $permissions['node:content_domain:delete own annvid content'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  // Exported og permission: 'node:content_domain:update any annvid content'
  $permissions['node:content_domain:update any annvid content'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  // Exported og permission: 'node:content_domain:update own annvid content'
  $permissions['node:content_domain:update own annvid content'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  return $permissions;
}
