<?php

namespace Page\node\domain;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of CreateDomainPage
 *
 * @author Bernhard
 */
class Create extends \Page\node\Node implements \Page\node\NodeCreateInterface {

    public function __construct(\AcceptanceTester $I) {
        parent::__construct($I, NM_CONTENT_DOMAIN);
    }

    public function fillFields(\AcceptanceTester $I, \Codeception\Example $params) {
        //body
        $I->fillCkEditorById('edit-field-domain-description-und-0-value', $params['body']);
    }

    /**
     * TODO....
     * @return int
     */
    public function getNewNid() {
        $I = $this->tester;
        $I->comment(__FUNCTION__ ." NOT IMPLEMENTED");
        return 23479232039;
    }

}