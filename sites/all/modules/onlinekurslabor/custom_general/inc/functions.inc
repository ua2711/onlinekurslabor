<?php

//22.04.2013 - 23:50 - SN
//helper function - special cases when Section Content should be active

function _section_domain_active($group) {

  //$node = menu_get_object();
  //_custom_general_get_group($group);
  //
  
  $section_content = _section_domain_author_section_active();

  if ($section_content) {
    return TRUE;
  }

  return FALSE;
}

function _section_course_active($group) {

  //$node = menu_get_object();
  //_custom_general_get_group($group);
  //
  
  $section_content = _section_course_instructor_section_active();

  if ($section_content) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Determines if user is viewing a content section which belongs to the backend
 * 
 * @return boolean 
 */
function _section_domain_author_section_active() {
  $book_active = (arg(0) == 'admin' && arg(1) == 'content' && arg(2) == 'book' );
  $biblio_active = (arg(0) == 'node' && arg(1) == 'add' && arg(2) == 'biblio' ); //not necessary ?
  $media_active = (arg(0) == 'domain' && arg(1) == 'media');
  $biblio_overview_active = (arg(0) == 'domain' && arg(1) == 'biblio');
  $tasks_overview_active = (arg(0) == 'domain' && arg(1) == 'tasks');

  return $biblio_active || $biblio_overview_active || $media_active || $book_active || $tasks_overview_active;
}

/**
 * Determines if user is viewing a course section which belongs to the instructor's backend
 * 
 * @return boolean 
 */
function _section_course_instructor_section_active() {
  /*
    $gc = og_context();
    $type_visible = FALSE;

    if($gc)
    {
    $active_node = node_load($gc['gid']);

    if(in_array($active_node->type, array(NM_COURSE, NM_COURSE_GROUP)))
    {
    $type_visible = TRUE;
    }
    }
   */
  $docs_visible = _section_course_course_docs_visible();
  $info_visible = (arg(0) == 'course' && arg(1) == 'info');
  $text_visible = _section_course_text_visible();
  $groups_visible = _section_course_course_group_visible();
  $members_visible = (arg(0) == 'course' && arg(1) == 'members');
  $forum_visible = _section_course_forum_visible();
  $tasks_visible = _section_course_course_tasks_visible();
  $admin_section_visible = (arg(0) == 'course' && arg(1) == 'admin');
  $home_visible = (arg(0) == 'course' && arg(1) == 'home');
  $news_visible = (arg(0) == 'course' && arg(1) == 'news');


  $section_courses = $docs_visible || $info_visible ||
      $groups_visible || $members_visible ||
      $text_visible || $forum_visible ||
      $admin_section_visible ||
      $home_visible || $tasks_visible ||
      $news_visible;

  if ($section_courses) {
    return TRUE;
  }

  return FALSE;
}

function _section_course_text_visible() {
  return $text_visible = (arg(0) == 'course' && arg(1) == 'text');
}

function _section_course_forum_visible() {
  $text_visible = (arg(0) == 'course' && arg(1) == 'forum');
  $text_visible = (arg(0) == 'course' && arg(1) == 'forumcontent') || $text_visible;
  return $text_visible;
}

function _section_course_course_group_visible() {
  $text_visible = (arg(0) == 'course' && arg(1) == 'groups');
  return $text_visible;
}

function _section_course_course_news_visible() {
  $text_visible = (arg(0) == 'course' && arg(1) == 'news');
  return $text_visible;
}

function _section_course_course_docs_visible() {
  $text_visible = (arg(0) == 'course' && (arg(1) == 'docs' || arg(1) == 'doc' ));
  return $text_visible;
}

function _section_course_course_tasks_visible() {
  $text_visible = (arg(0) == 'course' && arg(1) == 'tasks');
  return $text_visible;
}


/**
 * Get node type from the Database without overhead
 * @param type $nid
 * @return null
 */
function _custom_general_get_node_type($nid) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
      ->propertyCondition('nid', $nid);
  $results = $query->execute();

  if (count($results) > 0) {
    $node = current($results['node']);
    if (isset($node) && isset($node->type)) {
      return $node->type;
    }
  }
  return NULL;
}

/*
 * get num of new / unread content
 */

/**
 * Calculates the number of unread replies for each forum and returns the
 * count for the requested forum.
 */

/**
 * todo - choose course forum first
 * get unread num of posts in a forum
 * 
 * @staticvar null $result_cache
 * @param $uid user id
 * @param $uid term id
 * @return array
 */
function _section_courses_advanced_forum_get_num_unread($uid, $tid) {
  static $result_cache = NULL;
  $num_total = 0;


  if (is_null($result_cache)) {
    $result_cache = array();

    $query = db_select("comment", "c");
    $f_alias = $query->join("forum", "f", "c.nid = f.nid");
    //$n_alias = $query->join("node", "n", "f.vid = n.vid");
    $h_alias = $query->leftJoin("history", "h", "c.nid = h.nid AND h.uid = :uid", array(":uid" => $uid));
    $query->addExpression("COUNT(c.cid)", "count");
    $query->addField($f_alias, "tid");

    //forum name here...
    $query->condition("c.status", COMMENT_PUBLISHED)
        //fid
        ->condition("c.changed", NODE_NEW_LIMIT, ">")
        ->condition(db_or()->where("c.changed > {$h_alias}.timestamp")->isNull("h.timestamp"))
        ->groupBy("{$f_alias}.tid")
        ->addTag("node_access");

    $result = $query->execute();
    foreach ($result as $row) {
      $result_cache[$row->tid] = $row->count;
    }
  }

  //sum up
  $child_forums = array_keys(taxonomy_get_children($tid));

  foreach ($result_cache as $key => $row) {
    if (in_array($key, $child_forums)) {
      $num_total += $row;
    }
  }

  return $num_total;
}

function _section_courses_course_get_num_unread_by_type($uid, $type, $nid=NULL) {

  static $result_cache = array();

  if (empty($result_cache[$type]) || $nid != NULL) {
    //get group's news entities
    //$query = db_select("node", "n");
    $active_course = _section_courses_get_active_course();
    if(!$active_course)
       return $result_cache;
    $query = db_select("og_membership", "ogm");
    $query->condition("ogm.gid", $active_course->nid, "=");
    //$query->fields("ogm", array("entity_type", "etid"));
    $query->condition("ogm.entity_type", 'node', "=");
    $query->innerJoin("node", "n", "ogm.etid = n.nid");
    //$query->addField("ogm", "gid");
    //$query->addField("n", "nid");
    $query->condition('n.type', $type);
    
    if($nid && is_numeric($nid))
    {
      $query->condition('n.nid', $nid);
    }
    //history
    $h_alias = $query->leftJoin("history", "h", "n.nid = h.nid AND h.uid = :uid", array(":uid" => $uid));
    //$query->addField("h", "timestamp");
    //$query->addField("n", "changed");
    $query->addExpression("COUNT(n.nid)", "count");
    $query->condition("n.status", NODE_PUBLISHED)
        //fid
        ->condition("n.changed", NODE_NEW_LIMIT, ">")
        ->condition(db_or()->where("n.changed > {$h_alias}.timestamp")->isNull("h.timestamp"))
        //->groupBy("{$f_alias}.tid")
        ->addTag("node_access");

    $result = $query->execute();

    //dpm($result->fetchAll());
    $result_cache[$type] = current($result->fetchAll())->count;
  }

  return $result_cache[$type];

  //check history entries for every entity found
  //return unread
}

function custom_general_node_ids_by_type($type) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', $type);
  $results = $query->execute();
  return (array_keys($results['node']));
}

/**
 * get user's solution of a tasks
 * @param type $tid
 * @param type $uid
 * @return type
 */
function custom_general_get_task_solutions_by_uid($tid, $uid) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', NM_COURSE_GENERIC_TASK_SOLUTION)
      ->propertyCondition('status', 1)
      ->propertyCondition('uid', $uid)
      ->fieldCondition('field_generic_task', 'target_id', $tid, '=');

  $result = $query->execute();

  return isset($result['node']) ? $result['node'] : NULL;
}

/**
 * get user's solution of a tasks
 * dynamicly selection of user, depending on query arguments
 * 
 * @param type $tid
 * @param type $uid
 */
function custom_general_get_task_solutions($tid, $uid) {
  $uid_tmp = arg(5);
  if(!empty($uid_tmp) && is_numeric($uid_tmp))
  {
    $user = user_load($uid_tmp);
    if($user)
    {
      $uid = $user->uid;
    }
  }
  
  //check if group task or not
  
  
  return custom_general_get_task_solutions_by_uid($tid, $uid);
  
}